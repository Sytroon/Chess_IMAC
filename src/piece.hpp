#pragma once

#include <string>
#include <vector>

// Forward declaration to prevent circular dependencies
class Board;

// 2D coordinate on the chess board
struct Position {
    int x;
    int y;

    bool operator==(const Position& other) const { 
        return x == other.x && y == other.y; 
    }
};

// Color of a piece
enum class Color { 
    White, 
    Black 
};

// Abstract base class for all chess pieces ----------------------------------------------
class Piece {
protected:
    Color color;
    Position pos;
    std::string icon;

public:
    // Constructor for generic chess piece
    Piece(Color c, Position p, std::string icon) : color(c), pos(p), icon(icon) {}
    
    // Destructor
    virtual ~Piece() = default;

    // Return all possible position for a piece
    virtual std::vector<Position> getPossibleMoves(const Board& board) const = 0;

    // Getters and Setters
    Color getColor() const { return color; }
    Position getPos() const { return pos; }
    void setPos(Position p) { pos = p; }
    std::string getIcon() const { return icon; }
};

// Derived Piece Classes -----------------------------------------------------------------

class Pawn : public Piece {
public:
    Pawn(Color c, Position p);
    std::vector<Position> getPossibleMoves(const Board& board) const override;
};

class Knight : public Piece {
public:
    Knight(Color c, Position p);
    std::vector<Position> getPossibleMoves(const Board& board) const override;
};

class Bishop : public Piece {
public:
    Bishop(Color c, Position p);
    std::vector<Position> getPossibleMoves(const Board& board) const override;
};

class Rook : public Piece {
public:
    Rook(Color c, Position p);
    std::vector<Position> getPossibleMoves(const Board& board) const override;
};

class Queen : public Piece {
public:
    Queen(Color c, Position p);
    std::vector<Position> getPossibleMoves(const Board& board) const override;
};

class King : public Piece {
public:
    King(Color c, Position p);
    std::vector<Position> getPossibleMoves(const Board& board) const override;
};