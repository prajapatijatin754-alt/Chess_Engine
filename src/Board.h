#ifndef BOARD_H
#define BOARD_H

#include "ChessTypes.h"
#include "Piece.h"
#include <memory>
using std::unique_ptr;
class Board{
    private:
std::unique_ptr<Piece> grid[8][8];    
public:
    Board();
    ~Board()=default;
    void InitializeDefaultBoard();
    bool isWithinBounds(Position p)const{
        return p.row >= 0 && p.row < 8 && p.col >=0 && p.col < 8;
    }
    Piece* GetPieceAt(Position p)const;
    Position GetKingPosition(PieceColor color) const;
    bool isKingInCheck(PieceColor color) const;
    Piece* ExecuteMove(const Move&move);
    void UndoMove(const Move&move,Piece* removedPiece);
};
#endif