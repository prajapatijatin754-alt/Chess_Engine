#include "ChessGame.h"
using namespace std;
ChessGame::ChessGame(){
    currentTurn = PieceColor::White;
}

vector<Move> ChessGame::GetLegalMoves(Position p){
    vector<Move> possibleMoves;
    Piece* piece = board.GetPieceAt(p);
    
    if(piece == nullptr || piece->GetColor() != currentTurn){
        return possibleMoves;
    }
    possibleMoves = piece->GetPossibleMoves(board,p);
    return piece->GetLegalMoves(board,p,possibleMoves);
}
vector<Move> ChessGame::GetAllLegalMoves(){
    vector<Move> legalMoves;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            vector<Move> pieceMoves = GetLegalMoves({row, col});
            legalMoves.insert(legalMoves.end(), pieceMoves.begin(), pieceMoves.end());
        }
    }
    return legalMoves;
}
bool ChessGame::isKingInCheck(PieceColor color){
    Position kingPos = board.GetKingPosition(color);
    for(int r=0;r<8;r++){
        for(int c=0;c<8;c++){
            Position pos{r,c};
            if(board.GetPieceAt(pos)!=nullptr && board.GetPieceAt(pos)->GetColor()!=color){
                vector<Move> moves = board.GetPieceAt(pos)->GetPossibleMoves(board,pos);
                for(const Move& move : moves){
                    if(move.to == kingPos){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
bool ChessGame::AttemptMove(Position from, Position to, PieceType promotion){
    vector<Move> allowedMoves = GetLegalMoves(from);
    for(const Move& move : allowedMoves){
        if(move.to == to){
            if (move.promotionChoice != PieceType::NONE && promotion == PieceType::NONE) {
                return false;
            }
            Piece* captured = board.ExecuteMove(move);
            if (captured != nullptr) {
                delete captured;
            }
            Piece* movedPiece = board.GetPieceAt(to);
            if (movedPiece != nullptr) {
                movedPiece->SetMoved(true);
            }
            if (move.promotionChoice != PieceType::NONE) {
                if (!board.PromotePiece(move.to, promotion)) {
                    return false;
                }
            }
            if(move.isCastling){
                if(move.to.col == 6){
                    Position rookFrom = {move.from.row,7};
                    Position rookTo = {move.from.row,5};
                    board.ExecuteMove({rookFrom,rookTo});
                }
                if(move.to.col == 2){
                    Position rookFrom = {move.from.row,0};
                    Position rookTo = {move.from.row,3};
                    board.ExecuteMove({rookFrom,rookTo});
                }
            }
            SwitchTurn();
            return true;
        }
    }
    return false;
}

bool ChessGame::isGameOver(){
    Position kingPos=board.GetKingPosition(currentTurn);
    if(kingPos.row==-1){
        return true;
    }
    for(int r=0;r<8;r++){
        for(int c=0;c<8;c++){
            Position pos{r,c};
            if(board.GetPieceAt(pos)!=nullptr && board.GetPieceAt(pos)->GetColor()==currentTurn){
                vector<Move> moves = GetLegalMoves(pos);
                if(!moves.empty()){
                    return false;
                }
            }
        }
    }
    return true;
}
Position ChessGame::GetKingPosition(PieceColor color) const {
    return board.GetKingPosition(color);
}