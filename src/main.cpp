#include <iostream>
#include <string>
#include "board.hpp"
#include "player.hpp"
#include "quick_imgui/quick_imgui.hpp"

int main()
{
    // 1. Initialisation des données (AVANT la boucle)
    std::cout << "Veuillez entrer votre nom : ";
    std::string name;
    // std::cin >> name; // Commenté pour tester plus vite
    name = "Joueur 1";

    Player p1(true, name); // Supposons que Player est défini dans player.hpp

    // Création de l'échiquier UNE SEULE FOIS ici
    Board chessBoard;

    // 2. Lancement de l'interface
    quick_imgui::loop("Chess Game", {.init = [&]() {}, .loop = [&]() {
            // ImGui::Begin("Partie en cours");
            
            // ImGui::Text("Tour de : %s", p1.getName().c_str());
            // ImGui::Separator();

            // // On délègue tout l'affichage à la classe Board
            

            
            // --- Fenêtre de droite : Infos Joueurs ---
            ImGui::Begin("Informations");
            
            ImGui::Text("Joueur 1 (Blanc): Kasparov");
            ImGui::Text("Joueur 2 (Noir): Deep Blue");
            ImGui::Separator();

            // Affichage dynamique du tour
            ImGui::Text("Tour actuel : ");
            ImGui::SameLine();
            if (chessBoard.getCurrentTurn() == PieceColor::White) {
                ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "BLANCS"); // Vert
            } else {
                ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "NOIRS");  // Rouge
            }
            chessBoard.draw();

            ImGui::End(); }});

    return 0;
}