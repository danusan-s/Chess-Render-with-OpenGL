#ifndef CHESS_H
#define CHESS_H

#include "piece.h"
#include <iostream>
#include <string>
#include <vector>

enum class M_Result
{
    INVALID,
    VALID,
    CASTLING,
    EN_PASSANT
};

// Class to hold the main chessboard and run all operations
class Chessboard
{
    private:
        static const int SIZE = 8;
        Piece board[SIZE][SIZE];

        // Variables to keep track of the king's postions
        int blackKingRow;
        int blackKingCol;
        int whiteKingRow;
        int whiteKingCol;

        // Variable to select promotion type
        char promotionType;

        // Stores previous move (useful for enpassant) and perhaps animating moves
        std::vector<int> prevMove;

    public:
        //Initializes the chess board
        Chessboard();

        bool checkValidSource(int sourceRow,int sourceCol,bool black);

        /**
         * @brief Checks whether the user input is a valid move
         *
         * @param move contains the user's input
         * @param black indicates if it is black's turn
         * @return true when the input is valid
         * @return false when the input is invalid
         */
        bool movePiece(const std::vector<int>& move, bool black);

        /**
         * @brief Updates the private variables that store the king's position (note: does not move the king)
         *
         * @param row New row position of the king
         * @param col New column position of the king
         */
        void updateKingPosn(int row, int col);

        M_Result canBeReached(int sourceRow, int sourceCol, int destRow, int destCol);

        /**
         * @brief Determines whether a piece can reach from source to destination in one move
         *
         * @param sourceRow Row of the source
         * @param sourceCol Column of the source
         * @param destRow Row of the destination
         * @param destCol Column of the destination
         * @return true if the piece can reach the destination in one move
         * @return false if the piece cannot reach the destination in one move
         */
        M_Result isValidMove(int sourceRow, int sourceCol, int destRow, int destCol);

        /**
         * @brief Checks whether the king is in check
         *
         * @param black Indicates whether to look at Black or White's king
         * @return true if the king is in check
         * @return false if the king is safe
         */
        bool isKingInCheck(bool black);

        /**
         * @brief Checks whether the king is in checkmate
         *
         * @param black Indicates whether to look at Black or White's king
         * @return true if the king is in checkmate
         * @return false if the king can be saved using a move
         */
        bool hasNoPossibleMoves(bool black);
        /**
     * @brief Checks whether the move is valid for a pawn piece (Includes double move, En passant)
     *
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the pawn move is valid
     * @return false if the pawn move is invalid
     */
    M_Result isValidPawnMove(int sourceRow, int sourceCol, int destRow, int destCol);

    /**
     * @brief Checks whether the move is valid for a knight piece
     *
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the knight move is valid
     * @return false if the knight move is invalid
     */
    M_Result isValidKnightMove(int sourceRow, int sourceCol, int destRow, int destCol);

    /**
     * @brief Checks whether the move is valid for a bishop piece
     *
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the bishop move is valid
     * @return false if the bishop move is invalid
     */
    M_Result isValidBishopMove(int sourceRow, int sourceCol, int destRow, int destCol);

    /**
     * @brief Checks whether the move is valid for a rook piece
     *
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the rook move is valid
     * @return false if the rook move is invalid
     */
    M_Result isValidRookMove(int sourceRow, int sourceCol, int destRow, int destCol);

    /**
     * @brief Checks whether the move is valid for a queen piece (Combination of rook and bishop)
     *
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the queen move is valid
     * @return false if the queen move is invalid
     */
    M_Result isValidQueenMove(int sourceRow, int sourceCol, int destRow, int destCol);

    /**
     * @brief Checks whether the move is valid for a king piece (Includes castling)
     *
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the king move is valid
     * @return false if the king move is invalid or castling is under check
     */
    M_Result isValidKingMove(int sourceRow, int sourceCol, int destRow, int destCol);

    /**
     * @brief Sets the promotion type for the pawn
     *
     * @param type Character indicating the type of promotion (e.g., 'Q' for Queen)
     */
    void setPromotionType(char type);

    /**
     * @brief Gets the piece at the specified row and column
     *
     * @param row Row of the piece
     * @param col Column of the piece
     * @return Piece at the specified location
     */

    std::vector<int> getPreviousMove();

    Piece getPiece(int row, int col);
};

#endif

