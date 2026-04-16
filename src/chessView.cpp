#include "chessView.hpp"

void ChessView::draw(Game& game) {
    // Only display menu if in menu
    if (game.getState() == GameState::MainMenu) {
        drawMainMenu(game);
        return; 
    }

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("Chess Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Mode : %s", game.isRandomMode() ?  "Chaos" : "Normale");
    ImGui::Text("Tour : %s", game.getTurn() == Color::White ? "Blancs" : "Noirs");
    
    if (game.getState() == GameState::Promotion) ImGui::TextColored(ImVec4(1,1,0,1), "Promotion en cours...");

    ImGui::Spacing();
    if (ImGui::Button("Retour au menu")) {
        game.returnToMenu(); 
    }
    ImGui::Spacing();
    
    // Detect right-click to deselect the current piece
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        game.cancelSelection();
    }

    // Draw UI components
    drawGameOverPanel(game);
    drawPromotionPopup(game);
    drawBoardGrid(game);
}

void ChessView::drawMainMenu(Game& game) {
    // Centered
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
    
    if (ImGui::Begin("Menu Principal", nullptr, flags)) {
        ImGui::Text("Les échecs de Ilyass et Hugo");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Normal mode
        if (ImGui::Button("Mode Normal", ImVec2(500, 50))) {
            game.startGame(false);
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        // Random mode
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.9f, 1.0f));
        if (ImGui::Button("Mode Aleatoire", ImVec2(500, 50))) {
            game.startGame(true);
        }
        ImGui::PopStyleColor(2);

        ImGui::End();
    }
}

void ChessView::drawGameOverPanel(Game& game) {
    if (game.getState() == GameState::BlackWins || game.getState() == GameState::WhiteWins) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.04, 0.6f, 1.0f, 1.0f));
        
        if (game.getState() == GameState::WhiteWins) {
            ImGui::Text("VICTOIRE DES BLANCS !");
        } else {
            ImGui::Text("VICTOIRE DES NOIRS !");
        }
        
        ImGui::PopStyleColor();
        
        // Back to main menu
        if (ImGui::Button("Menu Principal")) {
            game.returnToMenu();
        }
        
        ImGui::Separator();
    }
}

void ChessView::drawPromotionPopup(Game& game) {
    if (game.getState() == GameState::Promotion) {
        ImGui::OpenPopup("Promotion");
    }

    if (ImGui::BeginPopupModal("Promotion", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Choisissez une piece pour la promotion :");
        ImGui::Separator();

        // String passed must match the ones in Game::promotePawn
        if (ImGui::Button("Dame")) { 
            game.promotePawn("Dame"); 
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Tour")) { 
            game.promotePawn("Tour"); 
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Fou")) { 
            game.promotePawn("Fou"); 
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Cavalier")) { 
            game.promotePawn("Cavalier"); 
            ImGui::CloseCurrentPopup(); 
        }

        ImGui::EndPopup();
    }
}

void ChessView::drawBoardGrid(Game& game) {
    const Board& board = game.getBoard();
    const auto& highlights = game.getHighlights();
    Piece* selected = game.getSelectedPiece();

    // Remove spacing and padding (no gaps between buttons)
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    for (int i = 0; i < Board::BOARD_SIZE; ++i) {
        for (int j = 0; j < Board::BOARD_SIZE; ++j) {
            Position currentPos{i, j};
            ImGui::PushID(i * Board::BOARD_SIZE + j);

            // COLOR LOGIC -------------------------------

            // A. Checkerboard pattern
            ImVec4 finalColor = ((i + j) % 2 == 0) 
                ? ImVec4(238.0f / 255.0f, 238.0f / 255.0f, 210.0f / 255.0f, 1.0f)   // Light square 
                : ImVec4(118.0f / 255.0f, 150.0f / 255.0f, 86.0f / 255.0f, 1.0f);  // Dark square

            // B. Highlight valid moves
            for (const auto& h : highlights) {
                if (h == currentPos) {
                    if (board.getPiece(currentPos) != nullptr) {
                        finalColor = ImVec4(1.0f, 0.2f, 0.2f, 0.7f); // RED: Capture possible
                    } else {
                        finalColor = ImVec4(1.0f, 0.8f, 0.2f, 0.5f); // YELLOW: Empty square move
                    }
                    break;
                }
            }

            // C. Highlight selected piece
            if (selected != nullptr && selected->getPos() == currentPos) {
                finalColor = ImVec4(0.2f, 0.6f, 1.0f, 0.8f); // BLUE: Selected
            }

            // BUTTON RENDERING ---------------------------

            ImGui::PushStyleColor(ImGuiCol_Button, finalColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(finalColor.x, finalColor.y, finalColor.z, 0.9f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(finalColor.x * 0.8f, finalColor.y * 0.8f, finalColor.z * 0.8f, 1.0f));

            Piece* p = board.getPiece(currentPos);
            std::string label = p ? p->getIcon() : "";

            // Pieces color ------------------------------
            bool hasPieceColor = false;
            if (p != nullptr) {
                hasPieceColor = true;
                if (p->getColor() == Color::Black) {
                    // Black
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); 
                } else {
                    // White
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); 
                }
            }

            // Button
            if (ImGui::Button(label.c_str(), ImVec2(60, 60))) {
                game.handleSquareClick(currentPos);
            }

            if (hasPieceColor) {
                ImGui::PopStyleColor();
            }

            // Keep buttons on the same row unless it's the last column
            if (j < Board::BOARD_SIZE - 1) {
                ImGui::SameLine();
            }

            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
    }

    // Restore ItemSpacing and FramePadding
    ImGui::PopStyleVar(2); 
}