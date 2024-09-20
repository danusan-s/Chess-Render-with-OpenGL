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
    public:
        GameLogicHandler();
        ~GameLogicHandler();
        void Init(bool isPlayerBlack,int BOT_LVL);
        Chessboard Board;
        bool blackTurn;
        int tileColorState[8][8];
        void clickSquare(int row,int col);
        void updateTileColorState();
        void resetTileColorState();
        bool handlePlayerMove(int srcR, int srcC, int destR, int destC);
        std::string getMoveFromStockfish();
        void makeStockfishMove();
        bool isCompleted();
};

#endif
