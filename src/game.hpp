#pragma once
#include "board.hpp"

enum class GameState { Playing,
                       Promotion,
                       WhiteWins,
                       BlackWins };

class Game {
private:
    Board                 board;
    Color                 turn          = Color::White;
    Piece*                selectedPiece = nullptr;
    std::vector<Position> validMoves;
    GameState             state = GameState::Playing; // État actuel
    Position              promotionPos;

    // Animation de déplacement
    bool                  pathAnimating = false;
    float                 pathTime      = 0.0f;     // Progression en secondes
    float                 pathDuration  = 1.2f;     // durée totale de l’animation
    Position              pathStart;
    Position              pathTarget;
    std::vector<Position> pathSquares;

public:
    void handleSquareClick(Position p);
    void cancelSelection();
    void reset(); // Pour le bouton "Recommencer"

    // Gestion animation
    bool                  isPathAnimating() const { return pathAnimating; }
    const std::vector<Position>& getPathSquares() const { return pathSquares; }
    float                 getPathTime() const { return pathTime; }
    float                 getPathDuration() const { return pathDuration; }
    void                  updatePathAnimation(float dt);
    void                  clearPathAnimation();
    void                  computePathSquares(Position from, Position to);
    bool                  isPathStaircase(Position from, Position to) const;

    // Getters
    const Board&                 getBoard() const { return board; }
    const std::vector<Position>& getHighlights() const { return validMoves; }
    Piece*                       getSelectedPiece() const { return selectedPiece; }
    Color                        getTurn() const { return turn; }
    GameState                    getState() const { return state; }
    void                         promotePawn(std::string choice);
    Position                     getPromotionPos() const { return promotionPos; }
};