#pragma once
#include <string>
#include <vector>

struct Position {
    int  x, y;
    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
};

enum class Color { White,
                   Black };

class Piece {
protected:
    Color       color;
    Position    pos;
    std::string icon;

public:
    Piece(Color c, Position p, std::string icon) : color(c), pos(p), icon(icon) {}
    virtual ~Piece() = default;

    virtual std::vector<Position> getPossibleMoves(const class Board& board) const = 0;

    Color       getColor() const { return color; }
    Position    getPos() const { return pos; }
    void        setPos(Position p) { pos = p; }
    std::string getIcon() const { return icon; }
};

// Exemple avec le Cavalier (Knight)
class Knight : public Piece {
public:
    Knight(Color c, Position p);
    std::vector<Position> getPossibleMoves(const class Board& board) const override;
};

class Pawn : public Piece {
public:
    using Piece::Piece; // Constructeur hérité
    Pawn(Color c, Position p);
    std::vector<Position> getPossibleMoves(const class Board& board) const override;
};

class Rook : public Piece {
public:
    Rook(Color c, Position p);
    std::vector<Position> getPossibleMoves(const class Board& board) const override;
};

class Bishop : public Piece {
public:
    Bishop(Color c, Position p);
    std::vector<Position> getPossibleMoves(const class Board& board) const override;
};

class Queen : public Piece {
public:
    Queen(Color c, Position p);
    std::vector<Position> getPossibleMoves(const class Board& board) const override;
};

class King : public Piece {
public:
    King(Color c, Position p);
    std::vector<Position> getPossibleMoves(const class Board& board) const override;
};