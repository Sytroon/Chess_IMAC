#pragma once

#include <vector>
#include <string>
#include "board.hpp"

// State of the game to update view
enum class GameState { 
    MainMenu,
    Playing,
    Promotion,
    WhiteWins,
    BlackWins 
};

// Controls rules, turns and animation
class Game {
private:
    // Game state
    bool m_isRandomMode = false;
    Board board;
    Color turn = Color::White;
    GameState state = GameState::MainMenu;
    
    // Selection and movement
    Piece* selectedPiece = nullptr;
    std::vector<Position> validMoves;
    Position promotionPos;

    // Randoms 
    float currentJumpHeight = 1.0f;
    float pathDuration = 2.0f; 
    float rBack;
    float gBack;
    float bBack;
    int currentSpins = 0;
    float randomScale;
    // float randomXDecoration;
    // float randomZDecoration;
    // float randomRotationDecoration;

    // Animation data
    bool pathAnimating = false;
    float pathTime = 0.0f;            // Current animation progress (in seconds)
    Position pathStart;
    Position pathTarget;
    std::vector<Position> pathSquares; // Squares traversed during the animation

public:
    bool isRandomMode() const { return m_isRandomMode; }
    void startGame(bool random);
    void returnToMenu();
    float getJumpHeight() const { return currentJumpHeight; }
    float getCurrentSpins() const { return currentSpins; }
    float getRBack() const { return rBack; }
    float getGBack() const { return gBack; }
    float getBBack() const { return bBack; }
    float getRandomScale() const { return randomScale; }
    // float getRandomXDecoration() const { return randomXDecoration; }
    // float getRandomZDecoration() const { return randomZDecoration; }
    // float getRandomRotationDecoration() const { return randomRotationDecoration; }

    // Resets the game to its initial state.
    void reset();

    // Game logic -----------------------------------------------------------------------

    // Handles a click from the player on square p
    void handleSquareClick(Position p);

    // Cancels the current piece selection and clears valid moves
    void cancelSelection();

    // Promotes a pawn to a the selected piece
    void promotePawn(const std::string& choice);

    // Animation Logic -----------------------------------------------------------------------

    void updatePathAnimation(float dt);
    void clearPathAnimation();
    void computePathSquares(Position from, Position to);
    bool isPathStaircase(Position from, Position to) const;

    // Getters -----------------------------------------------------------------------

    const Board& getBoard() const { return board; }
    const std::vector<Position>& getHighlights() const { return validMoves; }
    Piece* getSelectedPiece() const { return selectedPiece; }
    Color getTurn() const { return turn; }
    GameState getState() const { return state; }
    Position getPromotionPos() const { return promotionPos; }

    // Animation getters
    bool isPathAnimating() const { return pathAnimating; }
    const std::vector<Position>& getPathSquares() const { return pathSquares; }
    float getPathTime() const { return pathTime; }
    float getPathDuration() const { return pathDuration; }
    Position getPathStart() const { return pathStart; }
    Position getPathTarget() const { return pathTarget; }
};