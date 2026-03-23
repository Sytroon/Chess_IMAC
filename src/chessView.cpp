#include "chessView.hpp"
#include <imgui.h>
#include "game.hpp"

void ChessView::draw(Game& game)
{
    // 1. Détection du clic droit pour désélectionner
    // On vérifie si le clic droit est pressé ET si la fenêtre actuelle est survolée
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        game.cancelSelection();
    }

    // --- Interface de Fin de Partie ---
    if (game.getState() == GameState::BlackWins || game.getState() == GameState::WhiteWins)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1)); // Jaune pour le message
        if (game.getState() == GameState::WhiteWins)
            ImGui::Text("VICTOIRE DES BLANCS !");
        else
            ImGui::Text("VICTOIRE DES NOIRS !");
        ImGui::PopStyleColor();

        ImGui::SameLine();
        if (ImGui::Button("Recommencer"))
        {
            game.reset();
        }
        ImGui::Separator();
    }

    const Board& board      = game.getBoard();
    auto&        highlights = game.getHighlights();
    Piece*       selected   = game.getSelectedPiece();

    // 1. On supprime l'espace entre les éléments (Boutons)
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    // 2. Optionnel : On supprime les marges internes des boutons pour que l'icône soit bien centrée
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    // --- FENÊTRE DE PROMOTION ---
    if (game.getState() == GameState::Promotion)
    {
        ImGui::OpenPopup("Promotion");
    }

    if (ImGui::BeginPopupModal("Promotion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Choisissez une pièce pour la promotion :");
        ImGui::Separator();

        if (ImGui::Button("Dame"))
        {
            game.promotePawn("Dame");
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Tour"))
        {
            game.promotePawn("Tour");
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Fou"))
        {
            game.promotePawn("Fou");
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cavalier"))
        {
            game.promotePawn("Cavalier");
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            Position currentPos{i, j};
            ImGui::PushID(i * 8 + j);

            // --- LOGIQUE DE COULEUR ---

            // A. Couleur de base (Alternance bois)
            ImVec4 baseColor  = ((i + j) % 2 == 0) ? ImVec4(0.98f, 0.76f, 0.45f, 1.0f) : ImVec4(0.58f, 0.32f, 0.23f, 1.0f);
            ImVec4 finalColor = baseColor;

            // B. Coloration des mouvements possibles (Highlights)
            bool isPossibleMove = false;
            for (auto& h : highlights)
            {
                if (h == currentPos)
                {
                    isPossibleMove = true;
                    if (board.getPiece(currentPos) != nullptr)
                    {
                        // ROUGE : Capture possible
                        finalColor = ImVec4(1.0f, 0.2f, 0.2f, 0.7f);
                    }
                    else
                    {
                        // JAUNE : Case libre
                        finalColor = ImVec4(1.0f, 0.8f, 0.2f, 0.5f);
                    }
                    break;
                }
            }

            // C. Coloration de la pièce sélectionnée (Priorité sur le reste)
            if (selected != nullptr && selected->getPos() == currentPos)
            {
                finalColor = ImVec4(0.2f, 0.6f, 1.0f, 0.8f); // BLEU
            }

            // --- RENDU DU BOUTON ---

            ImGui::PushStyleColor(ImGuiCol_Button, finalColor);

            // Effet de survol légèrement plus clair que la couleur actuelle
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(finalColor.x, finalColor.y, finalColor.z, 0.9f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(finalColor.x * 0.8f, finalColor.y * 0.8f, finalColor.z * 0.8f, 1.0f));

            Piece*      p     = board.getPiece(currentPos);
            std::string label = p ? p->getIcon() : "";

            if (ImGui::Button(label.c_str(), ImVec2(60, 60)))
            {
                game.handleSquareClick(currentPos);
            }

            // Gestion du passage à la ligne
            if (j < 7)
            {
                ImGui::SameLine();
            }

            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
    }

    ImGui::PopStyleVar(2); // On restaure ItemSpacing et FramePadding
}