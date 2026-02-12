#pragma once // Remplace les #ifndef/#define, plus moderne
#include <string>
#include "piece.hpp" // Nécessaire pour connaître PieceColor

class Player {
private:
    std::string name;
    PieceColor color;
    bool isHuman;     // Pour plus tard : différencier IA et Humain
    int score;

public:
    // Constructeur : on garde la signature que tu avais dans le main (bool isWhite, string name)
    Player(bool isWhite, std::string playerName);

    // Accesseurs (Getters) pour récupérer les infos sans les modifier
    std::string getName() const;
    PieceColor getColor() const;
    bool isWhiteSide() const; // Retourne vrai si le joueur est blanc

    // Pour afficher des infos de debug ou GUI
    void printInfo() const;
};