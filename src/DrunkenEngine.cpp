#include "ChessGame.h"
#include "Board.h"
#include <vector>
#include <iostream>
using namespace std;
class DrunkenEngine : public ChessGame {
    public:
        DrunkenEngine() = default;
        ~DrunkenEngine() = default;
        
        Move GenerateRandomMove(const Board& board){
            vector<Move> legalmoves;
            for (int r = 0; r < 8; r++){
                for (int c = 0; c < 8; c++){
                    Position pos = {r,c};
                    if(board.GetPieceAt(pos) != nullptr && board.GetPieceAt(pos)->GetColor() == currentTurn){
                        vector<Move> moves = GetLegalMoves(pos);
                        legalmoves.insert(legalmoves.end(),moves.begin(),moves.end());
                    }
                }
            }
            if(legalmoves.empty()){
                return Move{{-1,-1},{-1,-1}};
            }
            int randomIndex = rand() % legalmoves.size();
            return legalmoves[randomIndex];
        }

        void MakeAIMove(){
            Move move = GenerateRandomMove(board);
            board.ExecuteMove(move);
            SwitchTurn();
        }
};