#ifndef CHESSGAME_H
#define CHESSGAME_H

#include "Board.h"
#include <vector>

class ChessGame:public Board{
    private:
        Board board;
        PieceColor currentTurn;
        void SwitchTurn(){
            currentTurn = (currentTurn == PieceColor::White) ? PieceColor::Black : PieceColor::White;
        }
    public:
        ChessGame();
        ~ChessGame()=default;
        const Board& GetBoard() const {return board;}
        PieceColor GetCurrentTurn() const {return currentTurn;}
        //get valid moves for a piece
        std::vector<Move> GetLegalMoves(Position);
        bool AttemptMove(Position, Position);
        bool isGameOver();
        bool isKingInCheck(PieceColor);
        Position GetKingPosition(PieceColor color) const;

};

#endif