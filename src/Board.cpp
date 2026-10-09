#include "Board.h"

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
        grid[1][c] = std::make_unique<Pawn>(PieceColor::Black);
        grid[6][c] = std::make_unique<Pawn>(PieceColor::White);
    }

    // 3. Set up Rooks (Corners)
    grid[0][0] = std::make_unique<Rook>(PieceColor::Black);
    grid[0][7] = std::make_unique<Rook>(PieceColor::Black);
    grid[7][0] = std::make_unique<Rook>(PieceColor::White);
    grid[7][7] = std::make_unique<Rook>(PieceColor::White);

    // 4. Set up Knights
    grid[0][1] = std::make_unique<Knight>(PieceColor::Black);
    grid[0][6] = std::make_unique<Knight>(PieceColor::Black);
    grid[7][1] = std::make_unique<Knight>(PieceColor::White);
    grid[7][6] = std::make_unique<Knight>(PieceColor::White);

    // 5. Set up Bishops
    grid[0][2] = std::make_unique<Bishop>(PieceColor::Black);
    grid[0][5] = std::make_unique<Bishop>(PieceColor::Black);
    grid[7][2] = std::make_unique<Bishop>(PieceColor::White);
    grid[7][5] = std::make_unique<Bishop>(PieceColor::White);

    // 6. Set up Queens
    grid[0][3] = std::make_unique<Queen>(PieceColor::Black);
    grid[7][3] = std::make_unique<Queen>(PieceColor::White);

    // 7. Set up Kings
    grid[0][4] = std::make_unique<King>(PieceColor::Black);
    grid[7][4] = std::make_unique<King>(PieceColor::White);
}

// Executes a move on the board
Piece* Board::ExecuteMove(const Move& move) {
    Piece* tobeRemoved = grid[move.to.row][move.to.col].release();
    // std::move transfers ownership of the pointer from one array slot to another.
    grid[move.to.row][move.to.col] = std::move(grid[move.from.row][move.from.col]);
    grid[move.from.row][move.from.col] = nullptr;
    return tobeRemoved;
}
void Board::UndoMove(const Move& move,Piece* removedPiece){
    grid[move.from.row][move.from.col] = std::move(grid[move.to.row][move.to.col]);
    grid[move.to.row][move.to.col] = nullptr;
    if(removedPiece!=nullptr){
        grid[move.to.row][move.to.col] = std::unique_ptr<Piece>(removedPiece);
    }
}       

bool Board::isKingInCheck(PieceColor color)const{
    Position kingpos = GetKingPosition(color);
    if(kingpos.row==-1){
        return true;
    }
    for(int r=0;r<8;r++){
        for(int c=0;c<8;c++){
            if(grid[r][c] != nullptr && grid[r][c]->GetColor()!=color){
                std::vector<Move> temp_move = grid[r][c]->GetPossibleMoves(*this,{r,c});
                for(Move m:temp_move){
                    if(m.to.row==kingpos.row && m.to.col==kingpos.col){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
