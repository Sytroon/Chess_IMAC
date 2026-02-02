#pragma once

#include <imgui.h>
#include <iostream>
#include <string>
#include <vector>
#include "quick_imgui/quick_imgui.hpp"

struct Position {
    int pos_x;
    int pos_y;
};

class Board {
private:
    int                                   size;
    std::vector<std::vector<bool>>        board;
    std::vector<std::vector<std::string>> visualBoard;

public:
    Board();
    void     initializeBoard();
    void     prepareBoard();
    Position checkClick();
    void     playTurn();
    Board&   operator=(bool&);
};