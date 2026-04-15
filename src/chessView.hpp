#pragma once

#include <imgui.h>
#include "game.hpp"

// Handles 2D UI
class ChessView {
public:
    // Draw UI components
    void draw(Game& game);

private:
    // End of game panel
    void drawGameOverPanel(Game& game);

    // Promotion panel
    void drawPromotionPopup(Game& game);

    // Interactive chessboard
    void drawBoardGrid(Game& game);
};