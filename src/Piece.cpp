#include "Piece.h"
#include "Board.h"

using namespace std;

vector<Move>Pawn::GetPossibleMoves(const Board& board,Position currentPosition)const{
    vector<Move> moves;
    int direction= (color == PieceColor::White) ? -1 : 1; //white moves up(row dec) black moves down(row inc)
    
    Position forwardOne = {currentPosition.row + direction, currentPosition.col};
    if(board.isWithinBounds(forwardOne) && board.GetPieceAt(forwardOne)==nullptr){
        moves.push_back(Move(currentPosition,forwardOne,type));
        if(!hasMoved){
            Position forwardTwo = {currentPosition.row + direction*2, currentPosition.col};
            if(board.isWithinBounds(forwardTwo) && board.GetPieceAt(forwardTwo)==nullptr){
                moves.push_back(Move(currentPosition,forwardTwo,type));
            }
        }
    }
    //diagonal captures
    int capturecols[2] = {currentPosition.col-1,currentPosition.col+1};
    for(int col : capturecols){
        Position diagPos = {currentPosition.row+direction,col};
        if(board.isWithinBounds(diagPos)){
            Piece* targetPiece = board.GetPieceAt(diagPos);
            if(targetPiece!=nullptr && targetPiece->GetColor() !=  this->color){
                moves.push_back(Move(currentPosition,diagPos,type,targetPiece->GetType()));
            }
        }
    }
    // A pawn promotes on the back rank. The move is marked so callers can
    // choose the replacement piece after the move is applied.
    int promotionRow = (color == PieceColor::White) ? 0 : 7;
    for (Move& move : moves) {
        if (move.to.row == promotionRow) {
            move.promotionChoice = PieceType::QUEEN;
        }
    }

    //En passant remains to be implemented.
    return moves;
}



vector<Move>Piece::GetLegalMoves(Board& board,Position currentPosition,vector<Move>posibleMoves){
    vector<Move>legalMoves;
    for(auto &m:posibleMoves){
        Piece* removed = board.ExecuteMove(m);
        if(!board.isKingInCheck(color)){
            legalMoves.push_back(m);
        }
        board.UndoMove(m,removed);
    }
    return legalMoves;
}
//Knight Moves
vector<Move> Knight::GetPossibleMoves(const Board& board,Position currentPos)const{
    vector<Move> moves;
    int rowoffset[] = {-2,-2,-1,-1,1,1,2,2};
    int coloffset[] = {-1,1,-2,2,-2,2,-1,1};
    for(int i=0;i<8;i++){
        Position targetPos = {currentPos.row+rowoffset[i],currentPos.col+coloffset[i]};
        if(board.isWithinBounds(targetPos)){
            Piece* targetPiece = board.GetPieceAt(targetPos);
            if(targetPiece==nullptr || targetPiece->GetColor()!=this->color){
                moves.push_back(Move(currentPos,targetPos,type,targetPiece?targetPiece->GetType():PieceType::NONE));
            }
        }
    }    
    return moves;
}

//Bishop Moves

vector<Move> Bishop::GetPossibleMoves(const Board& board,Position currentPos)const{
    vector<Move>moves;
    int rowDir[]={-1,-1,1,1};
    int colDir[]={-1,1,1,-1};
    for(int i=0;i<4;i++){
        for(int step = 1;step<8;step++){
            Position targetpos = {currentPos.row+rowDir[i]*step ,currentPos.col+colDir[i]*step};
            if(board.isWithinBounds(targetpos)){
                Piece* targetPiece = board.GetPieceAt(targetpos);
                if(targetPiece==nullptr){
                    moves.push_back(Move(currentPos, targetpos, this->type, PieceType::NONE));
                }else{
                    if(targetPiece->GetColor() != this->color){
                        moves.push_back(Move(currentPos,targetpos,this->type,targetPiece->GetType()));
                    }
                    break;
                }
            }
            else{
                break;
            }
        }    
    }
    return moves;
}
//Rook Moves

vector<Move> Rook::GetPossibleMoves(const Board& board,Position currentPos)const{
    vector<Move>moves;
    int rowDir[]={-1,1,0,0};
    int colDir[]={0,0,1,-1};
    for(int i=0;i<4;i++){
        for(int step = 1;step<8;step++){
            Position targetpos = {currentPos.row+rowDir[i]*step ,currentPos.col+colDir[i]*step};
            if(board.isWithinBounds(targetpos)){
                Piece* targetPiece = board.GetPieceAt(targetpos);
                if(targetPiece==nullptr){
                    moves.push_back(Move(currentPos, targetpos, this->type, PieceType::NONE));
                }else{
                    if(targetPiece->GetColor() != this->color){
                        moves.push_back(Move(currentPos,targetpos,this->type,targetPiece->GetType()));
                    }
                    break;
                }
            }
            else{
                break;
            }
        }    
    }
    return moves;
}   
//Queen Moves

vector<Move> Queen::GetPossibleMoves(const Board& board,Position currentPos)const{
    vector<Move>moves;
    int rowDir[]={-1,-1,-1,0,0,1,1,1};
    int colDir[]={-1,0,1,-1,1,-1,0,1};
    for(int i=0;i<8;i++){
        for(int step = 1;step<8;step++){
            Position targetpos = {currentPos.row+rowDir[i]*step ,currentPos.col+colDir[i]*step};
            if(board.isWithinBounds(targetpos)){
                Piece* targetPiece = board.GetPieceAt(targetpos);
                if(targetPiece==nullptr){
                    moves.push_back(Move(currentPos, targetpos, this->type, PieceType::NONE));
                }else{
                    if(targetPiece->GetColor() != this->color){
                        moves.push_back(Move(currentPos,targetpos,this->type,targetPiece->GetType()));
                    }
                    break;
                }
            }
            else{
                break;
            }
        }    
    }
    return moves;
}   
//King Moves

vector<Move> King::GetPossibleMoves(const Board& board,Position currentPos)const{
    vector<Move>moves;
    int rowDir[]={-1,-1,-1,0,0,1,1,1};
    int colDir[]={-1,0,1,-1,1,-1,0,1};
    for(int i=0;i<8;i++){
        for(int step = 1;step<2;step++){
            Position targetpos = {currentPos.row+rowDir[i]*step ,currentPos.col+colDir[i]*step};
            if(board.isWithinBounds(targetpos)){
                Piece* targetPiece = board.GetPieceAt(targetpos);
                if(targetPiece==nullptr){
                    moves.push_back(Move(currentPos, targetpos, this->type, PieceType::NONE));
                }else{
                    if(targetPiece->GetColor() != this->color){
                        moves.push_back(Move(currentPos,targetpos,this->type,targetPiece->GetType()));
                    }
                    break;
                }
            }
            else{
                break;
            }
        }    
    }
        //castleinglogic
    if(this->color == PieceColor::White && currentPos.row == 7 && currentPos.col == 4 && !hasMoved && !board.is_white_checked){
        //white kingside casteling
        //if this move is played rook should be played to its designated position

        if(board.GetPieceAt(Position{7,5})==nullptr && board.GetPieceAt(Position{7,6})==nullptr && board.GetPieceAt(Position{7,7})!=nullptr && board.GetPieceAt(Position{7,7})->GetType()==PieceType::ROOK && !board.GetPieceAt(Position{7,7})->HasMoved()){
            moves.push_back(Move(currentPos,{7,6},type,PieceType::NONE,true));
        }
    } 
    if(this->color == PieceColor::White && currentPos.row == 7 && currentPos.col == 4 && !hasMoved && !board.is_white_checked){
        //white queenside casteling
        if(board.GetPieceAt(Position{7,1})==nullptr && board.GetPieceAt(Position{7,2})==nullptr && board.GetPieceAt(Position{7,3})==nullptr && board.GetPieceAt(Position{7,0})!=nullptr && board.GetPieceAt(Position{7,0})->GetType()==PieceType::ROOK && !board.GetPieceAt(Position{7,0})->HasMoved()){
            moves.push_back(Move(currentPos,{7,2},type,PieceType::NONE,true));
        }
    }
    if(this->color == PieceColor::Black && currentPos.row == 0 && currentPos.col == 4 && !hasMoved && !board.is_black_checked){
        //black kingside casteling
        if(board.GetPieceAt(Position{0,5})==nullptr && board.GetPieceAt(Position{0,6})==nullptr && board.GetPieceAt(Position{0,7})!=nullptr && board.GetPieceAt(Position{0,7})->GetType()==PieceType::ROOK && !board.GetPieceAt(Position{0,7})->HasMoved()){
            moves.push_back(Move(currentPos,{0,6},type,PieceType::NONE,true));
        }
    }
    if(this->color == PieceColor::Black && currentPos.row == 0 && currentPos.col == 4 && !hasMoved && !board.is_black_checked){
        //black queenside casteling
        if(board.GetPieceAt(Position{0,1})==nullptr && board.GetPieceAt(Position{0,2})==nullptr && board.GetPieceAt(Position{0,3})==nullptr && board.GetPieceAt(Position{0,0})!=nullptr && board.GetPieceAt(Position{0,0})->GetType()==PieceType::ROOK && !board.GetPieceAt(Position{0,0})->HasMoved()){
            moves.push_back(Move(currentPos,{0,2},type,PieceType::NONE,true));
        }
    }

    return moves;
}   