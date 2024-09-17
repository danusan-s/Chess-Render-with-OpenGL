#include "game_logic.h"

GameLogicHandler::GameLogicHandler(bool isPlayerBlack){
    this->isPlayerBlack = isPlayerBlack;
    blackTurn = false;
    prevSelectedCol=-1;
    prevSelectedRow=-1;
    moveCommand = "position startpos moves";
    resetTileColorState();
    Stockfish.readOutput();
    Stockfish.sendCommand("uci");
    Stockfish.sendCommand("setoption name Skill Level value "+std::to_string(BOT_LVL));
    Stockfish.sendCommand("isready");
}

GameLogicHandler::~GameLogicHandler(){

}

void GameLogicHandler::clickSquare(int row,int col){
    resetTileColorState();
    if (prevSelectedRow!=-1 && prevSelectedCol!=-1 
            && handlePlayerMove(prevSelectedRow,prevSelectedCol,row,col)){
        prevSelectedRow=-1;
        prevSelectedCol=-1;
        return;
    }
    prevSelectedRow = row;
    prevSelectedCol = col;
    updateTileColorState();
}

void GameLogicHandler::updateTileColorState(){
    if (prevSelectedRow%2 == prevSelectedCol%2){
        tileColorState[prevSelectedRow][prevSelectedCol]=2;
    }
    else{
        tileColorState[prevSelectedRow][prevSelectedCol]=-2;
    }
    for (int i=0;i<8;++i){
        for (int j=0;j<8;++j){
            M_Result result = Board.isValidMove(prevSelectedRow,prevSelectedCol,i,j);
            if (result == M_Result::INVALID){
                continue;
            }
            if ((i%2)==(j%2)){
                if (Board.getPiece(i,j).getColor()!=Color::None || result==M_Result::EN_PASSANT){
                    tileColorState[i][j]=3;
                }
                else{
                    tileColorState[i][j]=2;
                }
            }
            else{
                if (Board.getPiece(i,j).getColor()!=Color::None || result==M_Result::EN_PASSANT){
                    tileColorState[i][j]=-3;
                }
                else{
                    tileColorState[i][j]=-2;
                }
            }
        }
    }
}

void GameLogicHandler::resetTileColorState(){
    for (int i=0;i<8;++i){
        for (int j=0;j<8;++j){
            if ((i%2)==(j%2)){
                tileColorState[i][j]=1;
            }
            else{
                tileColorState[i][j]=-1;
            }
        }
    }
}

std::string cordToStringMove(std::vector<int> move) {
    std::string res = "";
    res += char(move[1] + 'a'); 
    res += char(8 - move[0] + '0');
    res += char(move[3] + 'a'); 
    res+=char(8 - move[2] + '0');
    return res;
}

std::vector<int> stringToCordMove(std::string& move) {
    return { 8 - move[1] + '0',move[0] - 'a',8 - move[3] + '0',move[2] - 'a' };
}

bool GameLogicHandler::handlePlayerMove(int srcR, int srcC, int destR, int destC){
    if (isPlayerBlack!=blackTurn){
        return false;
    }
    std::cout<<"Attempting move from "<<srcR<<" "<<srcC<<" to "<<destR<<" "<<destC<<std::endl;
    if (Board.movePiece({srcR,srcC,destR,destC},isPlayerBlack)){
        blackTurn = !blackTurn;
        moveCommand += " " + cordToStringMove({srcR,srcC,destR,destC});
        return true;
    }
    return false;
}

std::string GameLogicHandler::getMoveFromStockfish() {
    std::string result = Stockfish.readOutput();
    size_t pos = result.rfind("bestmove ");
    // In case we failed to read the move
    while (pos==std::string::npos) {
        result = Stockfish.readOutput();
        pos = result.rfind("bestmove ");
    }
    if (pos+5>=result.size()) return result.substr(pos+9,4);
    std::string bestMove = result.substr(pos+9,5);
    std::cout << bestMove << std::endl;
    return bestMove;
}

void GameLogicHandler::makeStockfishMove(){
    if (moveCommand == "position startpos moves") Stockfish.sendCommand("position startpos");
    else Stockfish.sendCommand(moveCommand);
    Stockfish.sendCommand("go movetime 200");
    std::string bestMove = getMoveFromStockfish();
    std::vector<int> move = stringToCordMove(bestMove);
    if (bestMove[4]!=' ') {
        Board.setPromotionType(bestMove[4]);
        moveCommand+= " "+bestMove;
    }
    else{
        moveCommand+= " "+bestMove.substr(0,4); 
    }
    Board.movePiece(move,blackTurn);
    blackTurn = !blackTurn;
}
