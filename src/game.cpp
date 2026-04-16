#include "game.hpp"
#include <cmath>
#include "randomVar.hpp"

// Game logic -----------------------------------------------------------------------

void Game::startGame(bool random) {
    m_isRandomMode = random;
    reset(); // Nettoie le plateau
    state = GameState::Playing; // Lance la partie

    if (m_isRandomMode) {
        // Position of the random additional piece
        Position whitePose{getBinomiale(7, 0.5), getBinomiale(7, 0.5)};
        Position blackPose{getBinomiale(7, 0.5), getBinomiale(7, 0.5)};

        // Type of the random additional piece
        int typeIndex = getPoisson(1.2); 
        if (typeIndex > 4) typeIndex = 4; // Safety

        std::unique_ptr<Piece> whitePiece;
        std::unique_ptr<Piece> blackPiece;

        switch (typeIndex) {
            case 0: 
                whitePiece = std::make_unique<Pawn>(Color::White, whitePose);
                blackPiece = std::make_unique<Pawn>(Color::Black, blackPose);  
                break;
            case 1: 
                whitePiece = std::make_unique<Knight>(Color::White, whitePose);
                blackPiece = std::make_unique<Knight>(Color::Black, blackPose); 
                break;
            case 2: 
                whitePiece = std::make_unique<Bishop>(Color::White, whitePose);
                blackPiece = std::make_unique<Bishop>(Color::Black, blackPose);  
                break;
            case 3: 
                whitePiece = std::make_unique<Rook>(Color::White, whitePose);
                blackPiece = std::make_unique<Rook>(Color::Black, blackPose); 
                break;
            case 4: 
            default: 
                whitePiece = std::make_unique<Queen>(Color::White, whitePose);
                blackPiece = std::make_unique<Queen>(Color::Black, blackPose);  
                break;
        }
        board.setPiece(whitePose, std::move(whitePiece));
        board.setPiece(blackPose, std::move(blackPiece));

        // Randomly colored background
        rBack = (float)getUniforme(0.0, 1.0);
        gBack = (float)getUniforme(0.0, 1.0);
        bBack = (float)getUniforme(0.0, 1.0);

        // Random scale bonus for pieces
        randomScale = getExponentielle(1);
    } else {
        // Default values
        rBack = 0.0;
        gBack = 0.0;
        bBack = 0.0;
        randomScale = 0.0;
    }
}

void Game::returnToMenu() {
    state = GameState::MainMenu;
}

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
                // 1. Check for win condition
                Piece* target = board.getPiece(p);
                if (dynamic_cast<King*>(target) != nullptr) {
                    state = (turn == Color::White) ? GameState::WhiteWins : GameState::BlackWins;
                }

                // 2. Setup move animation
                // (Get animation's jump height and speed based on gamemode)
                if (isRandomMode()) {
                    currentJumpHeight = getGauss(1, 2); 
                    pathDuration = getWeibull(3,2);
                    currentSpins = getGeometrique(0.4);
                } else {
                    currentJumpHeight = 1.0f; 
                    pathDuration = 2.0f;
                    currentSpins = 0;
                }

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

    // Knights jump directly, no staircase logic for animation
    if (dynamic_cast<const Knight*>(selectedPiece) != nullptr || 
       (absDx == 2 && absDy == 1) || (absDx == 1 && absDy == 2)) {
        return false;
    }

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

    // Calculate staircase path squares for animation
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

        // Trigger pawn promotion
        if (isPawn && reachedEnd) {
            state = GameState::Promotion;
            promotionPos = pathTarget;
        } else {
            // Or pass turn normally
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