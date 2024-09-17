#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "chess.h"
#include "stockfish.h"

class GameLogicHandler{
    private:
        bool isPlayerBlack;
        int prevSelectedRow,prevSelectedCol;
        StockfishProcess Stockfish;
        std::string moveCommand;
        const int BOT_LVL =1;
    public:
        GameLogicHandler(bool isPlayerBlack);
        ~GameLogicHandler();
        Chessboard Board;
        bool blackTurn;
        int tileColorState[8][8];
        void clickSquare(int row,int col);
        void updateTileColorState();
        void resetTileColorState();
        bool handlePlayerMove(int srcR, int srcC, int destR, int destC);
        std::string getMoveFromStockfish();
        void makeStockfishMove();
};

#endif
