#pragma once
#include <array>
#include <memory>
#include "Piece.hpp"

class Board {
private:
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> grid;

public:
    Board();
    void   reset();
    Piece* getPiece(Position p) const;
    void   movePiece(Position from, Position to);
    bool   isInside(Position p) const { return p.x >= 0 && p.x < 8 && p.y >= 0 && p.y < 8; }
    void   setPiece(Position p, std::unique_ptr<Piece> newPiece);
};