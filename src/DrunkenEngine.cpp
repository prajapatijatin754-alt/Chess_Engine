#include "ChessGame.h"
#include "Board.h"
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

class DrunkenEngine : public ChessGame {
private:
    const int SEARCH_DEPTH = 1; 

    // --- PIECE SQUARE TABLES (Positional Awareness) ---
    const int pawnPST[8][8] = {
        {  0,  0,  0,  0,  0,  0,  0,  0},
        { 50, 50, 50, 50, 50, 50, 50, 50},
        { 10, 10, 20, 30, 30, 20, 10, 10},
        {  5,  5, 10, 25, 25, 10,  5,  5},
        {  0,  0,  0, 20, 20,  0,  0,  0},
        {  5, -5,-10,  0,  0,-10, -5,  5},
        {  5, 10, 10,-20,-20, 10, 10,  5},
        {  0,  0,  0,  0,  0,  0,  0,  0}
    };

    const int knightPST[8][8] = {
        {-50,-40,-30,-30,-30,-30,-40,-50},
        {-40,-20,  0,  0,  0,  0,-20,-40},
        {-30,  0, 10, 15, 15, 10,  0,-30},
        {-30,  5, 15, 20, 20, 15,  5,-30},
        {-30,  0, 15, 20, 20, 15,  0,-30},
        {-30,  5, 10, 15, 15, 10,  5,-30},
        {-40,-20,  0,  5,  5,  0,-20,-40},
        {-50,-40,-30,-30,-30,-30,-40,-50}
    };

    const int kingPST[8][8] = {
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-20,-30,-30,-40,-40,-30,-30,-20},
        {-10,-20,-20,-20,-20,-20,-20,-10},
        { 20, 20,  0,  0,  0,  0, 20, 20},
        { 20, 30, 10,  0,  0, 10, 30, 20}
    };

    vector<Move> GetAllLegalMoves(PieceColor color) {
        vector<Move> legalmoves;
        
        // FORCE the turn so underlying GetLegalMoves doesn't block AI evaluation
        PieceColor actualTurn = currentTurn;
        currentTurn = color; 

        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                Position pos = {r, c};
                auto piece = board.GetPieceAt(pos);
                if (piece != nullptr && piece->GetColor() == color) {
                    vector<Move> moves = GetLegalMoves(pos);
                    legalmoves.insert(legalmoves.end(), moves.begin(), moves.end());
                }
            }
        }
        
        // RESTORE the turn
        currentTurn = actualTurn; 
        return legalmoves;
    }

    int EvaluateBoard() {
        int score = 0;
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                auto piece = board.GetPieceAt({r, c});
                if (piece != nullptr) {
                    bool isWhite = (piece->GetColor() == PieceColor::White);
                    int tableRow = isWhite ? r : (7 - r);
                    
                    int pieceValue = 0;
                    int positionalBonus = 0;

                    switch (piece->GetType()) {
                        case PieceType::PAWN:   
                            pieceValue = 100; 
                            positionalBonus = pawnPST[tableRow][c];
                            break;
                        case PieceType::KNIGHT: 
                            pieceValue = 320; 
                            positionalBonus = knightPST[tableRow][c];
                            break;
                        case PieceType::BISHOP: 
                            pieceValue = 330; 
                            positionalBonus = 10; 
                            break;
                        case PieceType::ROOK:   
                            pieceValue = 500; 
                            break;
                        case PieceType::QUEEN:  
                            pieceValue = 900; 
                            break;
                        case PieceType::KING:   
                            pieceValue = 20000; 
                            positionalBonus = kingPST[tableRow][c];
                            break;
                    }

                    int totalValue = pieceValue + positionalBonus;
                    if (isWhite) {
                        score += totalValue;
                    } else {
                        score -= totalValue;
                    }
                }
            }
        }
        return score;
    }

    int Minimax(int depth, int alpha, int beta, bool isMaximizingPlayer) {
        if (depth == 0) return EvaluateBoard();

        PieceColor activeColor = isMaximizingPlayer ? PieceColor::White : PieceColor::Black;
        vector<Move> moves = GetAllLegalMoves(activeColor);

        if (moves.empty()) return isMaximizingPlayer ? -99999 : 99999; 

        // Cache state
        PieceColor originalTurn = currentTurn; 

        if (isMaximizingPlayer) {
            int maxEval = -1000000;
            for (const Move& move : moves) {
                Piece* capturedPiece = board.GetPieceAt(move.to); 
                board.ExecuteMove(move);               
                
                // Force sync turn for the next depth
                currentTurn = PieceColor::Black; 
                int eval = Minimax(depth - 1, alpha, beta, false); 
                
                board.UndoMove(move, capturedPiece); 
                // Force restore state
                currentTurn = originalTurn; 
                
                maxEval = max(maxEval, eval);
                alpha = max(alpha, eval);
                if (beta <= alpha) break; 
            }
            return maxEval;
        } else {
            int minEval = 1000000;
            for (const Move& move : moves) {
                Piece* capturedPiece = board.GetPieceAt(move.to); 
                board.ExecuteMove(move);
                
                // Force sync turn for the next depth
                currentTurn = PieceColor::White;
                int eval = Minimax(depth - 1, alpha, beta, true);
                
                board.UndoMove(move, capturedPiece);
                // Force restore state
                currentTurn = originalTurn; 
                
                minEval = min(minEval, eval);
                beta = min(beta, eval);
                if (beta <= alpha) break; 
            }
            return minEval;
        }
    }

    Move GetBestMove() {
        vector<Move> legalmoves = GetAllLegalMoves(currentTurn);
        if (legalmoves.empty()) return Move{{-1,-1},{-1,-1}};

        Move bestMove = legalmoves[0];
        bool isWhiteTurn = (currentTurn == PieceColor::White);
        
        int bestScore = isWhiteTurn ? -1000000 : 1000000;
        int alpha = -1000000;
        int beta = 1000000;
        
        // Cache original turn state for the root call
        PieceColor originalTurn = currentTurn;

        for (const Move& move : legalmoves) {
            Piece* capturedPiece = board.GetPieceAt(move.to); 
            
            board.ExecuteMove(move);
            
            currentTurn = isWhiteTurn ? PieceColor::Black : PieceColor::White;
            int score = Minimax(SEARCH_DEPTH - 1, alpha, beta, !isWhiteTurn); 
            
            board.UndoMove(move, capturedPiece);
            currentTurn = originalTurn; 

            if (isWhiteTurn) {
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = move;
                }
                alpha = max(alpha, score);
            } else {
                if (score < bestScore) {
                    bestScore = score;
                    bestMove = move;
                }
                beta = min(beta, score);
            }
        }
        return bestMove;
    }

public:
    DrunkenEngine() = default;
    ~DrunkenEngine() = default;

    void MakeAIMove() {
        Move bestMove = GetBestMove();
        
        // Final sanity check so the engine doesn't crash if it can't find a move
        if (bestMove.to.row != -1 && bestMove.to.col != -1) {
            board.ExecuteMove(bestMove);
        }
        
        SwitchTurn();
    }
};