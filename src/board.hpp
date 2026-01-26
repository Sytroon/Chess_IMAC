#pragma once

#include <imgui.h>
#include <iostream>
#include <string>
#include "quick_imgui/quick_imgui.hpp"

class Board {
private:
    int                            size;
    std::vector<std::vector<bool>> board;

public:
    Board();
    void   initializeBoard();
    void   updateBoard();
    Board& operator=(bool&);
};