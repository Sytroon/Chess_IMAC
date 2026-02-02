#pragma once

#include <imgui.h>
#include <iostream>
#include <string>
#include "board.hpp"
#include "quick_imgui/quick_imgui.hpp"

class Piece {
private:
    bool        color; // noir = false, white = true
    std::string label;
    Position    position;

public:
    Piece(bool color, std::string const& label, Position const& pos);
    void     setPosition(Position const& pos);
    Position getPosition();
    void     move();
};