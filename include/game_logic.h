#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "chess.h"
#include "stockfish.h"

class GameLogicHandler{
    private:
        bool blackTurn;
        bool isPlayerBlack;
        int prevSelectedRow,prevSelectedCol;
        StockfishProcess Stockfish;
        std::string moveCommand;
    public:
        GameLogicHandler(bool isPlayerBlack);
        ~GameLogicHandler();
        Chessboard Board;
        void clickSquare(int row,int col);
        bool handleMove(int srcR, int srcC, int destR, int destC);
        std::string getMoveFromStockfish();
        void makeStockfishMove();
};

#endif
