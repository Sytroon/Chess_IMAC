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
            ImGui::Begin("Partie en cours");
            
            ImGui::Text("Tour de : %s", p1.getName().c_str());
            ImGui::Separator();

            // On délègue tout l'affichage à la classe Board
            chessBoard.draw();

            ImGui::End(); }});

    return 0;
}