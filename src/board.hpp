#pragma once

#include <array>
#include <memory>
#include "piece.hpp"

// Chessboard
class Board {
public:
    // Dimensions of the chessboard
    static constexpr int BOARD_SIZE = 8;

private:
    // 2D grid storing unique pointers to the pieces
    std::array<std::array<std::unique_ptr<Piece>, BOARD_SIZE>, BOARD_SIZE> grid;

public:
    // Default constructor
    Board();

    // Clears the board and resets all pieces to their initial starting positions (for replay at the end of a game)
    void reset();

    // Get a piece from its position
    Piece* getPiece(Position p) const;

    // Moves a piece from one square to another, updates its internal position
    void movePiece(Position from, Position to);

    // Return True if a piece (from its position) is inside of the board
    bool isInside(Position p) const { 
        return p.x >= 0 && p.x < BOARD_SIZE && p.y >= 0 && p.y < BOARD_SIZE; 
    }

    // Place a new piece on the board
    void setPiece(Position p, std::unique_ptr<Piece> newPiece);
};