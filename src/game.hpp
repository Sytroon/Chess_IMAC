#pragma once

#include <vector>
#include <string>
#include "board.hpp"

// State of the game to update view
enum class GameState { 
    Playing,
    Promotion,
    WhiteWins,
    BlackWins 
};

// Controls rules, turns and animation
class Game {
private:
    // Game state
    Board board;
    Color turn = Color::White;
    GameState state = GameState::Playing;
    
    // Selection and movement
    Piece* selectedPiece = nullptr;
    std::vector<Position> validMoves;
    Position promotionPos;

    // Animation data
    bool pathAnimating = false;
    float pathTime = 0.0f;            // Current animation progress (in seconds)
    float pathDuration = 2.0f;        // Total duration of a move animation
    Position pathStart;
    Position pathTarget;
    std::vector<Position> pathSquares; // Squares traversed during the animation

public:
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