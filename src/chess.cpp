#include "chess.h"

Chessboard::Chessboard(){
    promotionType = 'q';
    blackKingRow = 0;
    blackKingCol = 4;
    whiteKingRow = 7;
    whiteKingCol = 4;

    for (int i = 0; i < SIZE; ++i)
    {
        board[1][i] = Piece(Type::Pawn, Color::Black);
        board[6][i] = Piece(Type::Pawn, Color::White);
        for (int j = 2; j < 6; ++j)
        {
            board[j][i] = Piece();
        }
    }
    board[0][0] = Piece(Type::Rook, Color::Black);
    board[0][7] = Piece(Type::Rook, Color::Black);
    board[7][0] = Piece(Type::Rook, Color::White);
    board[7][7] = Piece(Type::Rook, Color::White);

    board[0][1] = Piece(Type::Knight, Color::Black);
    board[0][6] = Piece(Type::Knight, Color::Black);
    board[7][1] = Piece(Type::Knight, Color::White);
    board[7][6] = Piece(Type::Knight, Color::White);

    board[0][2] = Piece(Type::Bishop, Color::Black);
    board[0][5] = Piece(Type::Bishop, Color::Black);
    board[7][2] = Piece(Type::Bishop, Color::White);
    board[7][5] = Piece(Type::Bishop, Color::White);

    board[0][3] = Piece(Type::Queen, Color::Black);
    board[7][3] = Piece(Type::Queen, Color::White);

    board[0][4] = Piece(Type::King, Color::Black);
    board[7][4] = Piece(Type::King, Color::White);
}

bool Chessboard::checkValidSource(int sourceRow,int sourceCol,bool black) {
    // Check if there is a piece at the source square
    if (board[sourceRow][sourceCol].getType() == Type::None)
    {
        std::cout << "Invalid move. There is no piece at the source square." << std::endl;
        return false;
    }

    // Check if the piece at the source square is the correct colour based on turn
    if ((black && (board[sourceRow][sourceCol].getColor() == Color::White)) || (!black && (board[sourceRow][sourceCol].getColor() == Color::Black))) {
        std::cout << "Invalid move. This piece is not yours." << std::endl;
        return false;
    }

    return true;
}

bool Chessboard::movePiece(const std::vector<int>& move,bool black){
    // Validate input length
    if (move.size() != 4)
    {
        std::cout << "Invalid move format. Please use 4 coord system" << std::endl;
        return false;
    }

    // Extract source and destination squares
    int sourceRow = move[0];
    int sourceCol = move[1];
    int destRow = move[2];
    int destCol = move[3];

    // Check if source and destination squares are within the board
    if (sourceRow < 0 || sourceRow >= SIZE || sourceCol < 0 || sourceCol >= SIZE ||
            destRow < 0 || destRow >= SIZE || destCol < 0 || destCol >= SIZE)
    {
        std::cout << "Invalid move. Source or destination square is out of bounds." << std::endl;
        return false;
    }

    if (!checkValidSource(sourceRow,sourceCol,black)) return false;

    M_Result result = isValidMove(sourceRow, sourceCol, destRow, destCol);

    if (result!= M_Result::INVALID) {

        // If the move is valid, update the board
        Piece temp = board[destRow][destCol];
        board[destRow][destCol] = board[sourceRow][sourceCol];
        board[sourceRow][sourceCol] = Piece();

        // If the king moved, update king position
        if (board[destRow][destCol].getType() == Type::King) {
            updateKingPosn(destRow, destCol);
        }

        if (result == M_Result::EN_PASSANT) {
            board[sourceRow][destCol] = Piece();
        }
        else if (result == M_Result::CASTLING) {
            int col = destCol == 2 ? 0 : 7;
            int dcol = destCol == 2 ? 3 : 5;
            Piece rook = board[sourceRow][col];
            rook.setMoved();
            board[sourceRow][dcol] = rook;
            board[sourceRow][col] = Piece();
            return true;
        }
        // If pawn reaches either end of ranks, it can promote to another piece
        else if (board[destRow][destCol].getType() == Type::Pawn && (destRow == 0 || destRow == 7)) {
            Color pawnColor = board[destRow][destCol].getColor();
            switch (promotionType) {
                case 'q':
                    board[destRow][destCol] = Piece(Type::Queen, pawnColor);
                    break;
                case 'r':
                    board[destRow][destCol] = Piece(Type::Rook, pawnColor);
                    break;
                case 'n':
                    board[destRow][destCol] = Piece(Type::Knight, pawnColor);
                    break;
                case 'b':
                    board[destRow][destCol] = Piece(Type::Bishop, pawnColor);
                    break;
                default:
                    std::cout << "Invalid Promotion" << std::endl;
            }
        }

        // We just moved a piece to destRow, destCol so we set is as moved
        board[destRow][destCol].setMoved();

        // Store the move into prevMove
        prevMove = move;

        if (hasNoPossibleMoves(!black)) {
            completed=true;
            if (isKingInCheck(!black)) {
                std::cout << "Game over! " << (black ? "Black" : "White") << " Wins!" << std::endl;
            }
            else {
                std::cout << "Stalemate!" << std::endl;
            }
        }

        // The move is valid and didn't run into any obstructions
        return true;
    }
    else {
        std::cout << "That move is invalid" << std::endl;
    }
    return false;
}

void Chessboard::updateKingPosn(int row,int col){
    // The king has to be in the updated posn to retrieve the color
    // i.e. updateKingPosn after updating the board
    if (board[row][col].getColor() == Color::Black) {
        blackKingRow = row;
        blackKingCol = col;
    }
    else {
        whiteKingRow = row;
        whiteKingCol = col;
    }
}

M_Result Chessboard::canBeReached(int sourceRow, int sourceCol, int destRow, int destCol){
    // Can't move to the same square as source
    if (sourceRow == destRow && sourceCol == destCol) return M_Result::INVALID;

    // Handling move checking based on piece type
    switch (board[sourceRow][sourceCol].getType()) {
        case Type::Pawn:
            return (isValidPawnMove(sourceRow, sourceCol, destRow, destCol));
        case Type::Knight:
            return (isValidKnightMove(sourceRow, sourceCol, destRow, destCol));
        case Type::Bishop:
            return (isValidBishopMove(sourceRow, sourceCol, destRow, destCol));
        case Type::Rook:
            return (isValidRookMove(sourceRow, sourceCol, destRow, destCol));
        case Type::Queen:
            return (isValidQueenMove(sourceRow, sourceCol, destRow, destCol));
        case Type::King:
            return (isValidKingMove(sourceRow, sourceCol, destRow, destCol));
        default:
            break;
    }
    return M_Result::INVALID;
}

M_Result Chessboard::isValidMove(int sourceRow,int sourceCol, int destRow, int destCol){

    M_Result result = canBeReached(sourceRow,sourceCol,destRow,destCol);
    if (result==M_Result::INVALID) return M_Result::INVALID;

    bool black = board[sourceRow][sourceCol].getColor() == Color::Black;

    // If the move is valid, update the board
    Piece temp = board[destRow][destCol];
    board[destRow][destCol] = board[sourceRow][sourceCol];
    board[sourceRow][sourceCol] = Piece();

    // If the king moved, update king position
    if (board[destRow][destCol].getType() == Type::King) {
        updateKingPosn(destRow, destCol);
    }

    // If the king is in check after the move, it is invalid and we should revert back
    // Bug : if enemy puts you in check, you are able to play a move that puts them in check while you are being checked.
    if (isKingInCheck(black)) {
        result = M_Result::INVALID;
    }

    // Revert sequence
    board[sourceRow][sourceCol] = board[destRow][destCol];
    board[destRow][destCol] = temp;
    if (board[sourceRow][sourceCol].getType() == Type::King) {
        updateKingPosn(sourceRow, sourceCol);
    }

    return result;
}

bool Chessboard::isKingInCheck(bool black) {
    // Iterate through all pieces and see if they can attack the king
    // Same color pieces will be returned false by isValidMove
    if (black) {
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                if (board[i][j].getColor() != Color::White) continue;
                if (canBeReached(i, j, blackKingRow, blackKingCol)!=M_Result::INVALID) {
                    return true;
                }
            }
        }
    }
    else {
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                if (board[i][j].getColor() != Color::Black) continue;
                if (canBeReached(i, j, whiteKingRow, whiteKingCol)!= M_Result::INVALID) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Chessboard::hasNoPossibleMoves(bool black) {
    // Iterate through all the squares to find source
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {

            // Ignore pieces that we can't move in the current turn
            if (black && board[i][j].getColor() != Color::Black) continue;
            if (!black && board[i][j].getColor() != Color::White) continue;

            // Iterate through all the squares to find a destination
            for (int k = 0; k < SIZE; ++k) {
                for (int l = 0; l < SIZE; ++l) {

                    // Check if the piece can move to the destination
                    if (isValidMove(i, j, k, l)!= M_Result::INVALID) {

                        // Update the board
                        Piece temp = board[k][l];
                        board[k][l] = board[i][j];
                        board[i][j] = Piece();
                        // If the king is moved, update king position
                        if (board[k][l].getType() == Type::King) updateKingPosn(k, l);

                        // Check if the move makes the king safe
                        if (!isKingInCheck(black)) {
                            // Revert position as we do not want the player to know the safe move
                            board[i][j] = board[k][l];
                            board[k][l] = temp;
                            if (board[i][j].getType() == Type::King) updateKingPosn(i, j);
                            return false;
                        }

                        // If king is not safe, revert and try next iteration.
                        board[i][j] = board[k][l];
                        board[k][l] = temp;
                        if (board[i][j].getType() == Type::King) updateKingPosn(i, j);
                    }
                }
            }
        }
    }
    // None of the moves put the king out of check
    return true;
}

M_Result Chessboard::isValidPawnMove(int sourceRow, int sourceCol, int destRow, int destCol) {
    Piece sourcePiece = board[sourceRow][sourceCol];
    Piece destPiece = board[destRow][destCol];
    Color playerColor = sourcePiece.getColor();

    if (destPiece.getColor() == playerColor) return M_Result::INVALID;

    int direction = (playerColor == Color::White) ? -1 : 1;

    if (sourceCol == destCol && destPiece.getType() == Type::None) {
        if (destRow == sourceRow + direction) return M_Result::VALID;
        if (destRow == sourceRow + 2 * direction && !sourcePiece.hasPieceMoved() && board[sourceRow + direction][sourceCol].getType() == Type::None) return M_Result::VALID;
        return M_Result::INVALID;
    }

    if ((destCol == sourceCol + 1 || destCol == sourceCol - 1) && destRow == sourceRow + direction) {
        if (destPiece.getType() != Type::None) return M_Result::VALID;

        if (board[sourceRow][destCol].getType() == Type::Pawn && board[sourceRow][destCol].getColor() != playerColor) {
            if (prevMove[0] == prevMove[2] + 2 * direction && prevMove[1] == prevMove[3] && prevMove[1] == destCol) {
                return M_Result::EN_PASSANT;
            }
        }
    }
    return M_Result::INVALID;
}

M_Result Chessboard::isValidKnightMove(int sourceRow, int sourceCol, int destRow, int destCol) {
    Piece sourcePiece = board[sourceRow][sourceCol];
    Piece destPiece = board[destRow][destCol];
    Color playerColor = sourcePiece.getColor();

    if (destPiece.getColor() == playerColor) return M_Result::INVALID;

    if ((abs(destRow - sourceRow) == 2 && abs(destCol - sourceCol) == 1) || (abs(destRow - sourceRow) == 1 && abs(destCol - sourceCol) == 2)) {
        return M_Result::VALID;
    }

    return M_Result::INVALID;
}

M_Result Chessboard::isValidBishopMove(int sourceRow, int sourceCol, int destRow, int destCol) {
    Piece sourcePiece = board[sourceRow][sourceCol];
    Piece destPiece = board[destRow][destCol];
    Color playerColor = sourcePiece.getColor();

    if (destPiece.getColor() == playerColor) return M_Result::INVALID;

    int rowDiff = destRow - sourceRow;
    int colDiff = destCol - sourceCol;
    if (abs(rowDiff) != abs(colDiff)) return M_Result::INVALID;

    int stepRow = (rowDiff > 0) ? 1 : -1;
    int stepCol = (colDiff > 0) ? 1 : -1;
    int checkRow = sourceRow + stepRow;
    int checkCol = sourceCol + stepCol;

    while (checkRow != destRow && checkCol != destCol) {
        if (board[checkRow][checkCol].getType() != Type::None) return M_Result::INVALID;
        checkRow += stepRow;
        checkCol += stepCol;
    }
    return M_Result::VALID;
}

M_Result Chessboard::isValidRookMove(int sourceRow, int sourceCol, int destRow, int destCol) {
    Piece sourcePiece = board[sourceRow][sourceCol];
    Piece destPiece = board[destRow][destCol];
    Color playerColor = sourcePiece.getColor();

    if (destPiece.getColor() == playerColor) return M_Result::INVALID;

    if (destCol == sourceCol) {
        int direction = destRow > sourceRow ? 1 : -1;
        int checkRow = sourceRow + direction;
        while (checkRow != destRow) {
            if (board[checkRow][sourceCol].getType() != Type::None) return M_Result::INVALID;
            checkRow += direction;
        }
        return M_Result::VALID;
    }

    if (destRow == sourceRow) {
        int direction = destCol > sourceCol ? 1 : -1;
        int checkCol = sourceCol + direction;
        while (checkCol != destCol) {
            if (board[sourceRow][checkCol].getType() != Type::None) return M_Result::INVALID;
            checkCol += direction;
        }
        return M_Result::VALID;
    }

    return M_Result::INVALID;
}

M_Result Chessboard::isValidQueenMove(int sourceRow, int sourceCol, int destRow, int destCol) {
    return ((isValidBishopMove(sourceRow, sourceCol, destRow, destCol) == M_Result::VALID ||
             isValidRookMove(sourceRow, sourceCol, destRow, destCol) == M_Result::VALID) ?
            M_Result::VALID : M_Result::INVALID);
}

M_Result Chessboard::isValidKingMove(int sourceRow, int sourceCol, int destRow, int destCol) {
    Piece sourcePiece = board[sourceRow][sourceCol];
    Piece destPiece = board[destRow][destCol];
    Color playerColor = sourcePiece.getColor();

    if (destPiece.getColor() == playerColor) return M_Result::INVALID;

    int rowDiff = abs(destRow - sourceRow);
    int colDiff = abs(destCol - sourceCol);

    if (rowDiff <= 1 && colDiff <= 1) return M_Result::VALID;

    if (!sourcePiece.hasPieceMoved() && rowDiff == 0 && colDiff == 2) {
        if (destCol > sourceCol) {
            for (int col = sourceCol + 1; col < destCol; col++) {
                if (board[sourceRow][col].getType() != Type::None) return M_Result::INVALID;
            }
        } else {
            for (int col = sourceCol - 1; col > destCol; col--) {
                if (board[sourceRow][col].getType() != Type::None) return M_Result::INVALID;
            }
        }
        return M_Result::CASTLING;
    }

    return M_Result::INVALID;
}

void Chessboard::setPromotionType(char type) {
    promotionType = type;
}

std::vector<int> Chessboard::getPreviousMove(){
    return prevMove;
}

Piece Chessboard::getPiece(int row, int col) {
    return board[row][col];
}




