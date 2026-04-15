#include "game.hpp"
#include <cmath>

// Game logic -----------------------------------------------------------------------

void Game::reset() {
    board.reset();
    turn = Color::White;
    state = GameState::Playing;
    selectedPiece = nullptr;
    validMoves.clear();
    clearPathAnimation();
}

void Game::handleSquareClick(Position p) {
    // Block input if the game is over or an animation is currently playing
    if (state != GameState::Playing || pathAnimating) {
        return; 
    }

    Piece* clickedPiece = board.getPiece(p);

    // If a piece is already selected, check if the clicked square is a valid move
    if (selectedPiece) {
        for (const auto& move : validMoves) {
            if (move == p) {
                // 1. Check for win condition (King capture)
                Piece* target = board.getPiece(p);
                if (dynamic_cast<King*>(target) != nullptr) {
                    state = (turn == Color::White) ? GameState::WhiteWins : GameState::BlackWins;
                }

                // 2. Setup move animation (the actual board update happens when animation ends)
                pathAnimating = true;
                pathTime = 0.0f;
                pathStart = selectedPiece->getPos();
                pathTarget = p;
                computePathSquares(pathStart, pathTarget);

                return; // Wait for updatePathAnimation to finish the job
            }
        }
    }

    // Select a new piece if it belongs to the current player
    if (clickedPiece && clickedPiece->getColor() == turn) {
        selectedPiece = clickedPiece;
        validMoves = selectedPiece->getPossibleMoves(board);
    } else {
        cancelSelection();
    }
}

void Game::cancelSelection() {
    selectedPiece = nullptr;
    validMoves.clear();
}

void Game::promotePawn(const std::string& choice) {
    // The promoted piece belongs to the player who just played
    Color c = (turn == Color::White) ? Color::White : Color::Black;

    if (choice == "Dame") {
        board.setPiece(promotionPos, std::make_unique<Queen>(c, promotionPos));
    } else if (choice == "Tour") {
        board.setPiece(promotionPos, std::make_unique<Rook>(c, promotionPos));
    } else if (choice == "Fou") {
        board.setPiece(promotionPos, std::make_unique<Bishop>(c, promotionPos));
    } else if (choice == "Cavalier") {
        board.setPiece(promotionPos, std::make_unique<Knight>(c, promotionPos));
    }

    // Pass turn to the next player and resume play
    turn = (turn == Color::White) ? Color::Black : Color::White;
    state = GameState::Playing;
}

// Animation Logic -----------------------------------------------------------------------

bool Game::isPathStaircase(Position from, Position to) const {
    if (!selectedPiece) return false;

    int dx = to.x - from.x;
    int dy = to.y - from.y;
    int absDx = std::abs(dx);
    int absDy = std::abs(dy);

    // Knights jump directly, no staircase logic
    if (dynamic_cast<const Knight*>(selectedPiece) != nullptr || 
       (absDx == 2 && absDy == 1) || (absDx == 1 && absDy == 2)) {
        return false;
    }

    // Check for obstacles along the path. If an obstacle exists (excluding destination), 
    // disable staircase effect to avoid phasing through pieces
    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
    Position current{from.x + stepX, from.y + stepY};
    
    while (current.x != to.x || current.y != to.y) {
        if (board.getPiece(current) != nullptr) {
            return false;
        }
        current.x += stepX;
        current.y += stepY;
    }

    return true;
}

void Game::computePathSquares(Position from, Position to) {
    pathSquares.clear();
    
    if (from == to) return;

    if (!isPathStaircase(from, to)) {
        pathSquares.push_back(to);
        return;
    }

    // Calculate staircase path squares
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

    Position current = from;
    while (current.x != to.x || current.y != to.y) {
        current.x += stepX;
        current.y += stepY;
        pathSquares.push_back(current);
    }
}

void Game::updatePathAnimation(float dt) {
    if (!pathAnimating) return;

    pathTime += dt;

    // Check if animation has finished
    if (pathTime >= pathDuration) {
        board.movePiece(pathStart, pathTarget);

        Piece* movedPiece = board.getPiece(pathTarget);
        bool isPawn = (dynamic_cast<Pawn*>(movedPiece) != nullptr);
        bool reachedEnd = movedPiece && ((movedPiece->getColor() == Color::Black && pathTarget.x == 7) || 
                                         (movedPiece->getColor() == Color::White && pathTarget.x == 0));

        // Trigger pawn promotion if applicable
        if (isPawn && reachedEnd) {
            state = GameState::Promotion;
            promotionPos = pathTarget;
        } else {
            // Otherwise, switch turns normally
            turn = (turn == Color::White) ? Color::Black : Color::White;
        }

        // Clean up move state
        selectedPiece = nullptr;
        validMoves.clear();
        clearPathAnimation();
    }
}

void Game::clearPathAnimation() {
    pathAnimating = false;
    pathTime = 0.0f;
    pathSquares.clear();
}