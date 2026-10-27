//this file contains all the structs and Enums required to make the game engine working
#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

//enum for player turn
enum class PieceColor {
    None,
    White,
    Black
};

//enum for piece type
enum class PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE
};
struct Position {
    int row;
    int col;
    //bool operator<=>(const Position&) const = default;
    //in cpp 20+ we can use spaceship operator but for max compatibility i'll use == and !=
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

struct Move {
    Position from;
    Position to;

    PieceType pieceMoved;
    PieceType pieceCaptured;

    bool isCastling;
    bool isEnPassant;
    
    PieceType promotionChoice;

    Move() : 
        from{-1, -1}, to{-1, -1}, 
        pieceMoved(PieceType::NONE), pieceCaptured(PieceType::NONE),
        isCastling(false), isEnPassant(false), promotionChoice(PieceType::NONE) {}

    // Parameterized constructor for easy move creation
    Move(Position start, Position end, PieceType moved = PieceType::NONE, 
         PieceType captured = PieceType::NONE, bool castling = false, 
         bool enPassant = false, PieceType promotion = PieceType::NONE)
        : from(start), to(end), pieceMoved(moved), pieceCaptured(captured),
          isCastling(castling), isEnPassant(enPassant), promotionChoice(promotion) {}
};

#endif // CHESS_TYPES_H
