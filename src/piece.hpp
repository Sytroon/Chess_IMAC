#pragma once

#include <imgui.h>
#include <iostream>
#include <string>
#include "quick_imgui/quick_imgui.hpp"

struct Position {
    int pos_x;
    int pos_y;
};

class Piece {
private:
    bool        color; // noir = false, white = true
    std::string label;
    Position    position;

public:
    Piece(bool color, std::string const& label, Position const& pos);
    void     setPosition(Position const& pos);
    Position getPosition();
};