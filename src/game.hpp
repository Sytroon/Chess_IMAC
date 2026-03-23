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

public:
    void handleSquareClick(Position p);
    void cancelSelection();
    void reset(); // Pour le bouton "Recommencer"

    // Getters
    const Board&                 getBoard() const { return board; }
    const std::vector<Position>& getHighlights() const { return validMoves; }
    Piece*                       getSelectedPiece() const { return selectedPiece; }
    Color                        getTurn() const { return turn; }
    GameState                    getState() const { return state; }
    void                         promotePawn(std::string choice);
    Position                     getPromotionPos() const { return promotionPos; }
};