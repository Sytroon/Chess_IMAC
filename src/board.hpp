#pragma once
#include <vector>
#include <array>
#include "piece.hpp"

// Une structure simple pour définir une direction ou un décalage
struct Offset { int x; int y; };

class Board {
private:
    std::array<Piece, 64> squares;
    std::vector<int> possibleMoves;
    int selectedSquareIndex = -1;

    // --- FONCTIONS UTILITAIRES D'OPTIMISATION ---
    
    // Tente d'ajouter un mouvement. Retourne 'false' si on doit s'arrêter (obstacle/bord)
    bool tryAddMove(int originIndex, int targetX, int targetY);

    // Pour Tour, Fou, Reine : scanne dans une liste de directions
    void generateSlidingMoves(int index, const std::vector<Offset>& directions);

    // Pour Cavalier, Roi : vérifie une liste de cases cibles
    void generateSteppingMoves(int index, const std::vector<Offset>& offsets);

    // Pour le Pion (cas spécial)
    void generatePawnMoves(int index);

public:
    Board();
    void initialize();
    void draw();
    void movePiece(int from, int to);
    void calculatePossibleMoves(int index);
};