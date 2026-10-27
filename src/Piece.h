#ifndef PIECE_H
#define PIECE_H

#include "ChessTypes.h"
#include <vector>
using namespace std;

class Board;

class Piece{
    protected:
    PieceColor color;
    PieceType type;
    bool hasMoved;
    public:
    Piece(PieceColor c , PieceType t):
    color(c) ,
    type(t) ,
    hasMoved(false) {};
    virtual ~Piece()= default;

    PieceColor GetColor()const{return color;} //get color of the piece
    PieceType GetType()const{return type;} //get type of the piece
    bool HasMoved()const{return hasMoved;} //gettter
    //for the moves which are requried a piece to be not moves (castleing , 2 moves of pawn , etc)
    void SetMoved(bool moved){hasMoved=moved;} //setter

    //get all possible moves is a purely virtual function
    virtual vector<Move> GetPossibleMoves(const Board& board,Position currentPosition)const=0;
    //get all legal moves is a function that will be implemented by the child classes
    vector<Move> GetLegalMoves(Board& board,Position currentPosition,std::vector<Move>posibleMoves);
};

class Pawn:public Piece {
    public: 
    Pawn(PieceColor color):Piece(color , PieceType::PAWN){}
    virtual vector<Move> GetPossibleMoves(const Board& board,Position currentPosition)const override;
};

class Knight:public Piece{
    public:
    Knight(PieceColor color):Piece(color , PieceType::KNIGHT){}
    virtual vector<Move> GetPossibleMoves(const Board& board,Position currentPosition)const override;
};

class Bishop:public Piece{
    public:
    Bishop(PieceColor color):Piece(color , PieceType::BISHOP){}
    virtual vector<Move> GetPossibleMoves(const Board& board,Position currentPosition)const override;
};

class Rook:public Piece{
    public:
    Rook(PieceColor color):Piece(color , PieceType::ROOK){}
    virtual vector<Move> GetPossibleMoves(const Board& board,Position currentPosition)const override;
};

class Queen:public Piece{
    public:
    Queen(PieceColor color):Piece(color , PieceType::QUEEN){}
    virtual vector<Move> GetPossibleMoves(const Board& board,Position currentPosition)const override;
};

class King:public Piece{
    public:
    King(PieceColor color):Piece(color , PieceType::KING){}
    virtual vector<Move> GetPossibleMoves(const Board& board,Position currentPosition)const override;
};

#endif
