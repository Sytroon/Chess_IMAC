#include <iostream>
#include "ChessView.hpp"
#include "Game.hpp"
#include "quick_imgui/quick_imgui.hpp"

// On déclare le pointeur de police en dehors pour y avoir accès dans la boucle
ImFont* chessFont = nullptr;

int main()
{
    Game      chessGame;
    ChessView view;

    quick_imgui::loop("Chess Project", {.init = [&]() {
            // Ici, le contexte ImGui est créé, on peut charger la police !
            ImGuiIO& io = ImGui::GetIO();
    
    // 1. Créer un constructeur de plage de glyphes (Glyph Ranges Builder)
    static ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    
    // Ajouter les caractères de base (Lettres, Chiffres, etc.)
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault()); 
    
    // Ajouter spécifiquement les échecs (\u2654 à \u265F)
    static const ImWchar chessRanges[] = { 0x2654, 0x265F, 0 };
    builder.AddRanges(chessRanges);
    
    // Construire la plage finale
    builder.BuildRanges(&ranges);

    // 2. Charger la police avec la plage combinée
    // Utilise bien ranges.Data pour passer le tableau de caractères
    chessFont = io.Fonts->AddFontFromFileTTF(
        "C:/Users/hugo1/.!!HUGO/Font/freeserif/freeserif.ttf", 
        40.0f, 
        nullptr, 
        ranges.Data
            ); }, .loop = [&]() {
            ImGui::Begin("Chess Board");

            // Affichage du tour
            // ImGui::Text("Tour : %s", chessGame.getTurn() == Color::White ? "Blancs" : "Noirs");
            
            // On applique la police si elle a bien été chargée
            if (chessFont) ImGui::PushFont(chessFont);
            
            // Affichage du tour
            ImGui::Text("Tour : %s", chessGame.getTurn() == Color::White ? "Blancs" : "Noirs");
            
            view.draw(chessGame);
            
            if (chessFont) ImGui::PopFont();

            ImGui::End(); }});

    return 0;
}