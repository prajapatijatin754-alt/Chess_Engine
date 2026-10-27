#include <windows.h>
#include <winhttp.h>
#include "ChessGame.h"
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#pragma comment(lib, "winhttp.lib")

namespace {

string MakeJson(const string& value) {
    string escaped;
    for (char character : value) {
        if (character == '\\') {
            escaped += "\\\\";
        } else if (character == '"') {
            escaped += "\\\"";
        } else if (character == '\n') {
            escaped += "\\n";
        } else if (character == '\r') {
            escaped += "\\r";
        } else {
            escaped += character;
        }
    }
    return escaped;
}

string ReadEnvironmentVariable(const char* name) {
    const char* value = getenv(name);
    return value == nullptr ? string() : string(value);
}

string ExtractContent(const string& response) {
    const string marker = "\"content\":\"";
    size_t start = response.find(marker);
    if (start == string::npos) {
        throw runtime_error("No msg");
    }

    start += marker.length();
    string content;
    bool escaped = false;
    for (size_t index = start; index < response.length(); ++index) {
        char character = response[index];
        if (escaped) {
            content += character == 'n' ? '\n' : character;
            escaped = false;
        } else if (character == '\\') {
            escaped = true;
        } else if (character == '"') {
            return content;
        } else {
            content += character;
        }
    }
    throw runtime_error("Unterminated msg");
}

string Trim(string value) {
    const string whitespace = " \t\r\n";
    size_t first = value.find_first_not_of(whitespace);
    if (first == string::npos) {
        return {};
    }
    size_t last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1);
}

wstring ToWide(const string& value) {
    if (value.empty()) {
        return {};
    }
    int length = MultiByteToWideChar(CP_UTF8, 0, value.data(),
                                     static_cast<int>(value.size()), nullptr, 0);
    if (length <= 0) {
        throw runtime_error("Could not convert text to UTF-16");
    }
    wstring result(length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.data(),
                        static_cast<int>(value.size()), &result[0], length);
    return result;
}

// api would be taken from here
string HttpPost(const string& apiKey, const string& body) {
    HINTERNET session = WinHttpOpen(
        L"ChessEngine/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (session == nullptr) {
        throw runtime_error("WinHttpOpen failed");
    }

    WinHttpSetTimeouts(session, 1500, 1500, 5000, 5000);
    HINTERNET connection = WinHttpConnect(
        session, L"api.groq.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (connection == nullptr) {
        WinHttpCloseHandle(session);
        throw runtime_error("WinHttpConnect failed");
    }

    HINTERNET request = WinHttpOpenRequest(
        connection, L"POST", L"/openai/v1/chat/completions",
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (request == nullptr) {
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        throw runtime_error("WinHttpOpenRequest failed");
    }

    wstring headers = L"Content-Type: application/json\r\nAuthorization: Bearer "
                         + ToWide(apiKey) + L"\r\n";
    BOOL sent = WinHttpSendRequest(
        request, headers.c_str(), static_cast<DWORD>(-1L),
        const_cast<char*>(body.data()), static_cast<DWORD>(body.size()),
        static_cast<DWORD>(body.size()), 0);
    if (!sent || !WinHttpReceiveResponse(request, nullptr)) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        throw runtime_error("Groq HTTP request failed");
    }

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE |
                        WINHTTP_QUERY_FLAG_NUMBER, nullptr, &statusCode,
                        &statusSize, nullptr);

    string response;
    DWORD available = 0;
    while (WinHttpQueryDataAvailable(request, &available) && available > 0) {
        string chunk(available, '\0');
        DWORD received = 0;
        if (!WinHttpReadData(request, &chunk[0], available, &received)) {
            WinHttpCloseHandle(request);
            WinHttpCloseHandle(connection);
            WinHttpCloseHandle(session);
            throw runtime_error("Could not read Groq response");
        }
        chunk.resize(received);
        response += chunk;
    }

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);

    if (statusCode < 200 || statusCode >= 300) {
        throw runtime_error("Groq returned HTTP status " +
                                 to_string(statusCode));
    }
    return response;
}

char Promotionsuffix(PieceType type) {
    switch (type) {
    case PieceType::QUEEN: return 'q';
    case PieceType::ROOK: return 'r';
    case PieceType::BISHOP: return 'b';
    case PieceType::KNIGHT: return 'n';
    default: return '\0';
    }
}

string SquareToUci(Position position) {
    string square;
    square += static_cast<char>('a' + position.col);
    square += static_cast<char>('8' - position.row);
    return square;
}

string MoveToUci(const Move& move) {
    string uci = SquareToUci(move.from) + SquareToUci(move.to);
    char suffix = Promotionsuffix(move.promotionChoice);
    if (suffix != '\0') {
        uci += suffix;
    }
    return uci;
}

} // namespace

class DrunkenEngine : public ChessGame {
public:
    DrunkenEngine() = default;
    ~DrunkenEngine() = default;

    Move GenerateBestMove() {
        vector<Move> legalMoves = GetAllLegalMoves();
        if (legalMoves.empty()) {
            return Move{{-1, -1}, {-1, -1}};
        }

        string apiKey = ReadEnvironmentVariable("GROQ_API_KEY");
        if (apiKey.empty()) {
            throw runtime_error("GROQ_API_KEY is not set");
        }

        string legalMoveText;
        for (size_t index = 0; index < legalMoves.size(); ++index) {
            if (index != 0) {
                legalMoveText += ' ';
            }
            legalMoveText += MoveToUci(legalMoves[index]);
        }

        string prompt =
            "Choose the strongest chess move. Return exactly one UCI move "
            "from the legal move list and nothing else. FEN: " + GetFEN() +
            " Legal moves: " + legalMoveText;
        string model = ReadEnvironmentVariable("GROQ_MODEL");
        if (model.empty()) {
            model = "llama-3.1-8b-instant";
        }

        string body = "{\"model\":\"" + MakeJson(model) +
            "\",\"temperature\":0,\"max_tokens\":8,\"messages\":[{\"role\":\"user\",\"content\":\"" +
            MakeJson(prompt) + "\"}]}";
        string response = HttpPost(apiKey, body);
        string selectedUci = Trim(ExtractContent(response));

        for (const Move& move : legalMoves) {
            if (MoveToUci(move) == selectedUci) {
                return move;
            }
        }
        throw runtime_error("Groq returned an illegal move: " +
                                 selectedUci);
    }

    void MakeAIMove() {
        Move move = GenerateBestMove();
        if (move.from.row == -1) {
            return;
        }

        Piece* captured = board.ExecuteMove(move);
        delete captured;
        if (move.promotionChoice != PieceType::NONE) {
            if (!board.PromotePiece(move.to, move.promotionChoice)) {
                throw runtime_error("No promo..");
            }
        }
        SwitchTurn();
    }
};
