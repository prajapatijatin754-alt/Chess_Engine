#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h> 
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
#include <string.h>
#include <exception>

// Include our new engine
#include "ChessGame.h" 
#include "DrunkenEngine.cpp"
using namespace Gdiplus;
using Gdiplus::Color;
using Gdiplus::Graphics;
using Gdiplus::Image;
using Gdiplus::Ok;
using Gdiplus::SolidBrush;
using Gdiplus::GdiplusStartup;
using Gdiplus::GdiplusStartupInput;

const int BOARD_SIZE = 8;
const int SQUARE_SIZE = 80;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // 1. ASSET POINTERS (Loaded only once)
    static Image *whitePawn = nullptr, *blackPawn = nullptr;
    static Image *whiteKnight = nullptr, *blackKnight = nullptr;
    static Image *whiteRook = nullptr, *blackRook = nullptr;
    static Image *whiteQueen = nullptr, *blackQueen = nullptr;
    static Image *whiteKing = nullptr, *blackKing = nullptr;
    static Image *whiteBishop = nullptr, *blackBishop = nullptr;

    // 2. OUR NEW OOP CHESS ENGINE
    // static ChessGame game; 
    static DrunkenEngine game;
    // 3. MOUSE TRACKING VARIABLES
    static bool hasSelection = false;
    static Position selectedPos = {-1, -1};
    static std::vector<Move> currentValidMoves;
    static bool choosingPromotion = false;
    static Position promotionFrom = {-1, -1};
    static Position promotionTo = {-1, -1};
    static PieceColor promotionColor = PieceColor::None;

    auto imageForPiece = [&](PieceColor color, PieceType type) -> Image* {
        if (color == PieceColor::White) {
            if(type == PieceType::BISHOP) return whiteBishop;
            if(type == PieceType::KNIGHT) return whiteKnight;
            if(type == PieceType::ROOK) return whiteRook;
            if(type == PieceType::QUEEN) return whiteQueen;
        } else if (color == PieceColor::Black) {
            if (type == PieceType::KNIGHT) return blackKnight;
            if (type == PieceType::BISHOP) return blackBishop;
            if (type == PieceType::ROOK) return blackRook;
            if (type == PieceType::QUEEN) return blackQueen;
        }
        return nullptr;
    };

    auto finishPromotion = [&](PieceType promotionType) {
        try {
            if (game.AttemptMove(promotionFrom, promotionTo, promotionType)) {
                choosingPromotion = false;
                promotionFrom = {-1, -1};
                promotionTo = {-1, -1};
                promotionColor = PieceColor::None;
                hasSelection = false;
                currentValidMoves.clear();
                game.MakeAIMove();
                InvalidateRect(hwnd, NULL, FALSE);
            }
        } catch (const std::exception& error) {
            MessageBoxA(hwnd, error.what(), "AI move failed", MB_OK | MB_ICONERROR);
        }
    };
    switch (uMsg)
    {
    case WM_CREATE:
        // Load images just once
        whitePawn = new Image(L"../assests/white-pawn.png");
        blackPawn = new Image(L"../assests/black-pawn.png");
        whiteKnight = new Image(L"../assests/white-knight.png");
        blackKnight = new Image(L"../assests/black-knight.png");
        whiteRook = new Image(L"../assests/white-rook.png");
        blackRook = new Image(L"../assests/black-rook.png");
        whiteQueen = new Image(L"../assests/white-queen.png");
        blackQueen = new Image(L"../assests/black-queen.png");
        whiteKing = new Image(L"../assests/white-king.png");
        blackKing = new Image(L"../assests/black-king.png");
        whiteBishop = new Image(L"../assests/white-bishop.png");
        blackBishop = new Image(L"../assests/black-bishop.png");
        return 0;

    case WM_DESTROY:
        // Clean up images
        delete whitePawn; delete blackPawn;
        delete whiteKnight; delete blackKnight;
        delete whiteRook; delete blackRook;
        delete whiteQueen; delete blackQueen;
        delete whiteKing; delete blackKing;
        delete whiteBishop; delete blackBishop;
        PostQuitMessage(0);
        return 0;

    case WM_LBUTTONDOWN:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);

        if (choosingPromotion) {
            const PieceType promotionTypes[] = {
                PieceType::QUEEN, PieceType::ROOK,
                PieceType::BISHOP, PieceType::KNIGHT
            };
            const int optionSize = 80;
            const int optionLeft = (BOARD_SIZE * SQUARE_SIZE - optionSize * 4) / 2;
            const int optionTop = (BOARD_SIZE * SQUARE_SIZE - optionSize) / 2;

            if (mouseY >= optionTop && mouseY < optionTop + optionSize &&
                mouseX >= optionLeft && mouseX < optionLeft + optionSize * 4) {
                int option = (mouseX - optionLeft) / optionSize;
                finishPromotion(promotionTypes[option]);
            }
            return 0;
        }

        Position clickedPos = { mouseY / SQUARE_SIZE, mouseX / SQUARE_SIZE };

        if (hasSelection) {
            // STATE 2: A piece is already selected. Did they click on a valid dot?
            bool clickedValidMove = false;
            for (const Move& m : currentValidMoves) {
                if (m.to == clickedPos) {
                    clickedValidMove = true;
                    break;
                }
            }

            if (clickedValidMove) {
                Move selectedMove;
                for (const Move& move : currentValidMoves) {
                    if (move.to == clickedPos) {
                        selectedMove = move;
                        break;
                    }
                }

                if (selectedMove.promotionChoice != PieceType::NONE) {
                    choosingPromotion = true;
                    promotionFrom = selectedMove.from;
                    promotionTo = selectedMove.to;
                    Piece* pawn = game.GetBoard().GetPieceAt(selectedMove.from);
                    promotionColor = pawn->GetColor();
                } else {
                    try {
                        if (game.AttemptMove(selectedPos, clickedPos)) {
                            hasSelection = false;
                            currentValidMoves.clear();
                            game.MakeAIMove();
                        }
                    } catch (const std::exception& error) {
                        MessageBoxA(hwnd, error.what(), "AI move failed",
                                    MB_OK | MB_ICONERROR);
                    }
                }
            } else {
                // They clicked somewhere else. Let's see if they clicked a different friendly piece.
                currentValidMoves = game.GetLegalMoves(clickedPos);
                
                if (!currentValidMoves.empty()) {
                    // They selected a new piece, update the selection
                    selectedPos = clickedPos;
                } else {
                    // They clicked an empty square or enemy piece that isn't a valid move. Cancel selection.
                    hasSelection = false;
                    currentValidMoves.clear();
                }
            }
        } 
        else {
            // STATE 1: No piece is selected yet. Try to select one.
            currentValidMoves = game.GetLegalMoves(clickedPos);
            
            if (!currentValidMoves.empty()) {
                hasSelection = true;
                selectedPos = clickedPos;
            }
        }

        // Force the window to redraw immediately to show/hide dots and pieces
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    // case WM_LBUTTONUP:
    // {
    //     if (isDragging) {
    //         int mouseX = GET_X_LPARAM(lParam);
    //         int mouseY = GET_Y_LPARAM(lParam);
    //         Position targetPos = { mouseY / SQUARE_SIZE, mouseX / SQUARE_SIZE };

    //         game.AttemptMove(selectedPos, targetPos);

    //         isDragging = false;
    //         ReleaseCapture();
            
    //         // <-- NEW: Clear the valid moves so the dots disappear
    //         currentValidMoves.clear(); 

    //         // Redraw the board
    //         InvalidateRect(hwnd, NULL, FALSE);
    //     }
    //     return 0;
    // }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HBRUSH hBrushLight = CreateSolidBrush(RGB(238, 238, 210));
        HBRUSH hBrushDark = CreateSolidBrush(RGB(118, 150, 86));
        
        Graphics graphics(hdc);
        // Get a reference to the board so we can read it
        const Board& currentBoard = game.GetBoard();

        for (int row = 0; row < BOARD_SIZE; ++row)
        {
            for (int col = 0; col < BOARD_SIZE; ++col)
            {
                // 1. Draw the Square
                RECT rect = {
                    col * SQUARE_SIZE, row * SQUARE_SIZE,
                    (col + 1) * SQUARE_SIZE, (row + 1) * SQUARE_SIZE
                };

                if ((row + col) % 2 == 0) FillRect(hdc, &rect, hBrushLight);
                else FillRect(hdc, &rect, hBrushDark);

                // 2. Ask the engine what piece is here
                Position pos = {row, col};
                Piece* piece = currentBoard.GetPieceAt(pos);

                if (piece != nullptr) {
                    Image* imgToDraw = nullptr;
                    PieceColor color = piece->GetColor();
                    PieceType type = piece->GetType();

                    // Match the OOP piece to your GDI+ image pointers
                    if (color == PieceColor::White) {
                        if (type == PieceType::PAWN) imgToDraw = whitePawn;
                        else if (type == PieceType::KING) imgToDraw = whiteKing;
                    } else if (color == PieceColor::Black) {
                        if (type == PieceType::PAWN) imgToDraw = blackPawn;
                        else if (type == PieceType::KING) imgToDraw = blackKing;
                    }
                    if (imgToDraw == nullptr) {
                        imgToDraw = imageForPiece(color, type);
                    }

                    // Draw the image
                    if (imgToDraw && imgToDraw->GetLastStatus() == Ok) {
                        graphics.DrawImage(imgToDraw, col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
                    }
                }
            }
        }
        if (choosingPromotion) {
            const PieceType promotionTypes[] = {
                PieceType::QUEEN, PieceType::ROOK,
                PieceType::BISHOP, PieceType::KNIGHT
            };
            const int optionSize = 80;
            const int optionLeft = (BOARD_SIZE * SQUARE_SIZE - optionSize * 4) / 2;
            const int optionTop = (BOARD_SIZE * SQUARE_SIZE - optionSize) / 2;

            HBRUSH overlayBrush = CreateSolidBrush(RGB(245, 245, 245));
            RECT overlay = {optionLeft - 12, optionTop - 48,
                            optionLeft + optionSize * 4 + 12, optionTop + optionSize + 48};
            FillRect(hdc, &overlay, overlayBrush);
            DeleteObject(overlayBrush);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(30, 30, 30));
            RECT titleRect = {optionLeft - 12, optionTop - 38,
                              optionLeft + optionSize * 4 + 12, optionTop - 12};
            DrawTextW(hdc, L"Choose promotion", -1, &titleRect,
                      DT_CENTER | DT_SINGLELINE);

            for (int i = 0; i < 4; ++i) {
                RECT optionRect = {optionLeft + i * optionSize, optionTop,
                                   optionLeft + (i + 1) * optionSize, optionTop + optionSize};
                HBRUSH optionBrush = CreateSolidBrush(
                    (i % 2 == 0) ? RGB(238, 238, 210) : RGB(118, 150, 86));
                FillRect(hdc, &optionRect, optionBrush);
                DeleteObject(optionBrush);

                Image* optionImage = imageForPiece(promotionColor, promotionTypes[i]);
                if (optionImage != nullptr && optionImage->GetLastStatus() == Ok) {
                    graphics.DrawImage(optionImage, optionRect.left, optionRect.top,
                                       optionSize, optionSize);
                }
            }
        }
        if (game.isGameOver()) {
            // 1. Change std::string to std::wstring and use wide string literals (L"...")
            std::wstring winner = (game.GetCurrentTurn() == PieceColor::White) ? L"Black" : L"White";
            // 2. Concatenate using wide characters
            std::wstring message = L"Game Over! " + winner + L" Wins!";
            
            // 3. Pass message.c_str() and a wide caption literal L"Checkmate!"
            MessageBox(hwnd, message.c_str(), L"Checkmate!", MB_OK);
            DestroyWindow(hwnd); 
        }
        if(game.isKingInCheck(PieceColor::White)){
            Position kingPos = game.GetKingPosition(PieceColor::White);
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
            RECT rect = { kingPos.col * SQUARE_SIZE, kingPos.row * SQUARE_SIZE,
                        (kingPos.col + 1) * SQUARE_SIZE, (kingPos.row + 1) * SQUARE_SIZE };
            FillRect(hdc, &rect, hBrush);
            Image* imgToDraw;
            if (game.isKingInCheck(PieceColor::White)) {
                imgToDraw = whiteKing;
            }
            else {
                imgToDraw = blackKing;
            }
            graphics.DrawImage(imgToDraw, kingPos.col * SQUARE_SIZE, kingPos.row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
            DeleteObject(hBrush);
        }
        if  (game.isKingInCheck(PieceColor::Black)){
            Position kingPos = game.GetKingPosition(PieceColor::Black);
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
            RECT rect = { kingPos.col * SQUARE_SIZE, kingPos.row * SQUARE_SIZE,
                        (kingPos.col + 1) * SQUARE_SIZE, (kingPos.row + 1) * SQUARE_SIZE };
            FillRect(hdc, &rect, hBrush);
            Image* imgToDraw;
            if (game.isKingInCheck(PieceColor::Black)) {
                imgToDraw = blackKing;
            }
            else {
                imgToDraw = whiteKing;
            }
            graphics.DrawImage(imgToDraw, kingPos.col * SQUARE_SIZE, kingPos.row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
            DeleteObject(hBrush);
        }
        if (!currentValidMoves.empty()) {
            // Create a semi-transparent black brush (Alpha=100, R=0, G=0, B=0)
            SolidBrush dotBrush(Color(100, 0, 0, 0)); 
            
            for (const Move& m : currentValidMoves) {
                // Calculate the exact center of the destination square
                int centerX = m.to.col * SQUARE_SIZE + (SQUARE_SIZE / 2);
                int centerY = m.to.row * SQUARE_SIZE + (SQUARE_SIZE / 2);
                
                // Set dot radius (about 1/6th of the square size)
                int radius = SQUARE_SIZE / 6; 
                
                // Draw the circle
                graphics.FillEllipse(&dotBrush, centerX - radius, centerY - radius, radius * 2, radius * 2);
            }
        }
        DeleteObject(hBrushLight);
        DeleteObject(hBrushDark);

        EndPaint(hwnd, &ps);
        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ... WinMain remains exactly the same! ...

// ... (WinMain remains exactly the same as your previous code) ...
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    const wchar_t CLASS_NAME[] = L"ChessboardWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    int clientSize = BOARD_SIZE * SQUARE_SIZE;
    RECT windowRect = {0, 0, clientSize, clientSize};

    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRect(&windowRect, dwStyle, FALSE);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Win32 Chessboard", dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}