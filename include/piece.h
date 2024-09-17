#ifndef PIECE_H
#define PIECE_H

// Enum to classify the type of pieces
enum class Type
{
    None,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

// Enum to classify the color of pieces
enum class Color
{
    None,
    Black,
    White
};

// Class to identify each piece
class Piece
{
private:
    Type type;
    Color color;
    bool hasMoved;

public:
    Piece(Type t = Type::None, Color c = Color::None) : type(t), color(c), hasMoved(false) {}

    // Returns the type of piece
    Type getType() const
    {
        return type;
    }

    // Returns the color of piece
    Color getColor() const
    {
        return color;
    }

    // Returns whether the piece has been moved before
    bool hasPieceMoved() {
        return hasMoved;
    }

    // Sets bool hasMoved to true
    void setMoved() {
        hasMoved = true;
    }
};
#endif
