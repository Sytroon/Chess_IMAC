#pragma once

#include <imgui.h>
#include <iostream>
#include <string>
#include "Piece.hpp"
#include "quick_imgui/quick_imgui.hpp"

class Player {
private:
    bool               color; // noir = false, white = true
    std::string        name;
    std::vector<Piece> pieces;

public:
    Player(bool color, std::string name);
    void     initializePieces();
    Position play(Position pos);
};