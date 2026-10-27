#ifndef CHESSGAME_H
#define CHESSGAME_H

#include "Board.h"
#include <string>
#include <vector>

class ChessGame{
    protected:
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
        std::vector<Move> GetAllLegalMoves() ;
        std::string GetFEN() const { return board.ToFEN(currentTurn); }
        bool AttemptMove(Position, Position, PieceType promotion = PieceType::NONE);
        bool isGameOver();
        bool isKingInCheck(PieceColor);
        Position GetKingPosition(PieceColor color) const;
};

#endif