#include "Board.h"
#include <string>
using namespace std;
// Constructor
Board::Board() {
    InitializeDefaultBoard();
}

// Safely returns a raw pointer to the piece at a given position.
// Returns nullptr if the square is empty or out of bounds.
Piece* Board::GetPieceAt(Position p) const {
    if (!isWithinBounds(p)) {
        return nullptr;
    }
    // .get() returns the raw pointer managed by the unique_ptr
    return grid[p.row][p.col].get(); 
}

string Board::ToFEN(PieceColor sideToMove) const {
    string fen;
    for (int row = 0; row < 8; ++row) {
        int emptySquares = 0;
        for (int col = 0; col < 8; ++col) {
            Piece* piece = grid[row][col].get();
            if (piece == nullptr) {
                ++emptySquares;
                continue;
            }

            if (emptySquares > 0) {
                fen += to_string(emptySquares);
                emptySquares = 0;
            }

            char symbol = ' ';
            switch (piece->GetType()) {
            case PieceType::PAWN: symbol = 'p'; break;
            case PieceType::KNIGHT: symbol = 'n'; break;
            case PieceType::BISHOP: symbol = 'b'; break;
            case PieceType::ROOK: symbol = 'r'; break;
            case PieceType::QUEEN: symbol = 'q'; break;
            case PieceType::KING: symbol = 'k'; break;
            default: break;
            }

            if (piece->GetColor() == PieceColor::White) {
                symbol = static_cast<char>(symbol - ('a' - 'A'));
            }
            fen += symbol;
        }

        if (emptySquares > 0) {
            fen += to_string(emptySquares);
        }
        if (row != 7) {
            fen += '/';
        }
    }

    fen += sideToMove == PieceColor::White ? " w " : " b ";
    fen += "- - 0 1";
    return fen;
}

bool Board::PromotePiece(Position position, PieceType promotionType) {
    if (!isWithinBounds(position) || grid[position.row][position.col] == nullptr ||
        grid[position.row][position.col]->GetType() != PieceType::PAWN) {
        return false;
    }

    if (promotionType != PieceType::KNIGHT && promotionType != PieceType::BISHOP &&
        promotionType != PieceType::ROOK && promotionType != PieceType::QUEEN) {
        return false;
    }

    PieceColor color = grid[position.row][position.col]->GetColor();
    switch (promotionType) {
    case PieceType::KNIGHT:
        grid[position.row][position.col] = make_unique<Knight>(color);
        break;
    case PieceType::BISHOP:
        grid[position.row][position.col] = make_unique<Bishop>(color);
        break;
    case PieceType::ROOK:
        grid[position.row][position.col] = make_unique<Rook>(color);
        break;
    case PieceType::QUEEN:
        grid[position.row][position.col] = make_unique<Queen>(color);
        break;
    default:
        return false;
    }

    grid[position.row][position.col]->SetMoved(true);
    return true;
}
//iterate oer complete board and find the position of the KING
// one upgrade can be made => store the position of king whenever the king is moved and use this functtion as a gettter
Position Board::GetKingPosition(PieceColor color) const{
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(grid[i][j] != nullptr && grid[i][j]->GetColor() == color && grid[i][j]->GetType() == PieceType::KING){
                return {i,j};
            }
        }
    }
    return {-1,-1};
}
// Sets up a standard chess game
void Board::InitializeDefaultBoard() {
    // 1. Clear the board first (important if we restart the game later)
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            grid[r][c] = nullptr;
        }
    }

    // 2. Set up Pawns (Row 1 for Black, Row 6 for White)
    for (int c = 0; c < 8; ++c) {
        grid[1][c] = make_unique<Pawn>(PieceColor::Black);
        grid[6][c] = make_unique<Pawn>(PieceColor::White);
    }

    // 3. Set up Rooks (Corners)
    grid[0][0] = make_unique<Rook>(PieceColor::Black);
    grid[0][7] = make_unique<Rook>(PieceColor::Black);
    grid[7][0] = make_unique<Rook>(PieceColor::White);
    grid[7][7] = make_unique<Rook>(PieceColor::White);

    // 4. Set up Knights
    grid[0][1] = make_unique<Knight>(PieceColor::Black);
    grid[0][6] = make_unique<Knight>(PieceColor::Black);
    grid[7][1] = make_unique<Knight>(PieceColor::White);
    grid[7][6] = make_unique<Knight>(PieceColor::White);

    // 5. Set up Bishops
    grid[0][2] = make_unique<Bishop>(PieceColor::Black);
    grid[0][5] = make_unique<Bishop>(PieceColor::Black);
    grid[7][2] = make_unique<Bishop>(PieceColor::White);
    grid[7][5] = make_unique<Bishop>(PieceColor::White);

    // 6. Set up Queens
    grid[0][3] = make_unique<Queen>(PieceColor::Black);
    grid[7][3] = make_unique<Queen>(PieceColor::White);

    // 7. Set up Kings
    grid[0][4] = make_unique<King>(PieceColor::Black);
    grid[7][4] = make_unique<King>(PieceColor::White);
}

// Executes a move on the board
Piece* Board::ExecuteMove(const Move& move) {
    Piece* tobeRemoved = grid[move.to.row][move.to.col].release();
    // move transfers ownership of the pointer from one array slot to another.
    grid[move.to.row][move.to.col] = std::move(grid[move.from.row][move.from.col]);
    grid[move.from.row][move.from.col] = nullptr;
    return tobeRemoved;
}
void Board::UndoMove(const Move& move,Piece* removedPiece){
    grid[move.from.row][move.from.col] = std::move(grid[move.to.row][move.to.col]);
    grid[move.to.row][move.to.col] = nullptr;
    if(removedPiece!=nullptr){
        grid[move.to.row][move.to.col] = unique_ptr<Piece>(removedPiece);
    }
}       

bool Board::isKingInCheck(PieceColor color){
    Position kingpos = GetKingPosition(color);
    if(kingpos.row==-1){
        return true;
    }
    for(int r=0;r<8;r++){
        for(int c=0;c<8;c++){
            if(grid[r][c] != nullptr && grid[r][c]->GetColor()!=color){
                vector<Move> temp_move = grid[r][c]->GetPossibleMoves(*this,{r,c});
                for(Move m:temp_move){
                    if(m.to.row==kingpos.row && m.to.col==kingpos.col){
                        if(color==PieceColor::White){
                            this->is_white_checked = true;
                        }
                        else{
                            this->is_black_checked = true;
                        }
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
