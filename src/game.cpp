#include "game.hpp"
#include <cmath>

void Game::handleSquareClick(Position p)
{
    if (state != GameState::Playing || pathAnimating)
        return; // Bloque le jeu si fini ou si animation en cours

    Piece* clickedPiece = board.getPiece(p);

    if (selectedPiece)
    {
        for (auto& m : validMoves)
        {
            if (m == p)
            {
                // 1. Vérification victoire (Roi pris)
                Piece* target = board.getPiece(p);
                if (target && (target->getIcon() == "\u2654" || target->getIcon() == "\u265A"))
                {
                    state = (turn == Color::White) ? GameState::WhiteWins : GameState::BlackWins;
                }

                // 2. Démarrage animation de déplacement (en attendant, on ne bouge pas encore la pièce)
                pathAnimating = true;
                pathTime      = 0.0f;
                pathStart     = selectedPiece->getPos();
                pathTarget    = p;
                computePathSquares(pathStart, pathTarget);

                // On garde selectedPiece et validMoves le temps de l'animation

                // Trigger effect later dans updatePathAnimation
                return;
            }
        }
    }

    if (clickedPiece && clickedPiece->getColor() == turn)
    {
        selectedPiece = clickedPiece;
        validMoves    = selectedPiece->getPossibleMoves(board);
    }
    else
    {
        selectedPiece = nullptr;
        validMoves.clear();
    }
}

void Game::cancelSelection()
{
    selectedPiece = nullptr;
    validMoves.clear();
}

void Game::reset()
{
    board.reset();
    turn          = Color::White;
    state         = GameState::Playing;
    selectedPiece = nullptr;
    validMoves.clear();
}

// Fonction pour remplacer le pion par la pièce choisie
void Game::promotePawn(std::string choice)
{
    Color c = (turn == Color::White) ? Color::White : Color::Black;

    // On remplace le pion dans la grille du Board
    if (choice == "Dame")
        board.setPiece(promotionPos, std::make_unique<Queen>(c, promotionPos));
    else if (choice == "Tour")
        board.setPiece(promotionPos, std::make_unique<Rook>(c, promotionPos));
    else if (choice == "Fou")
        board.setPiece(promotionPos, std::make_unique<Bishop>(c, promotionPos));
    else if (choice == "Cavalier")
        board.setPiece(promotionPos, std::make_unique<Knight>(c, promotionPos));

    // On change de tour et on reprend le jeu
    turn  = (turn == Color::White) ? Color::Black : Color::White;
    state = GameState::Playing;
}
bool Game::isPathStaircase(Position from, Position to) const
{
    if (!selectedPiece)
        return false;

    int dx = to.x - from.x;
    int dy = to.y - from.y;
    int absDx = std::abs(dx);
    int absDy = std::abs(dy);

    // Cavalier : saut obligatoire, pas d'escalier
    if (dynamic_cast<const Knight*>(selectedPiece) != nullptr || (absDx == 2 && absDy == 1) || (absDx == 1 && absDy == 2))
        return false;

    // Si un obstacle se trouve sur le chemin (hors destination), on n'applique pas l'escalier
    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
    Position current{from.x + stepX, from.y + stepY};
    while (current.x != to.x || current.y != to.y)
    {
        if (board.getPiece(current) != nullptr)
            return false;
        current.x += stepX;
        current.y += stepY;
    }

    return true;
}

void Game::computePathSquares(Position from, Position to)
{
    pathSquares.clear();
    if (from.x == to.x && from.y == to.y)
        return;

    if (!isPathStaircase(from, to))
    {
        pathSquares.push_back(to);
        return;
    }

    // Mouvement avec escalier
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

    Position current = from;
    while (current.x != to.x || current.y != to.y)
    {
        current.x += stepX;
        current.y += stepY;
        pathSquares.push_back(current);
    }
}

void Game::updatePathAnimation(float dt)
{
    if (!pathAnimating)
        return;

    pathTime += dt;

    if (pathTime >= pathDuration)
    {
        board.movePiece(pathStart, pathTarget);

        Piece* movedPiece = board.getPiece(pathTarget);
        bool   isPawn     = movedPiece && (movedPiece->getIcon() == "\u2659" || movedPiece->getIcon() == "\u265F");
        bool   reachedEnd = movedPiece && ((movedPiece->getColor() == Color::Black && pathTarget.x == 7) || (movedPiece->getColor() == Color::White && pathTarget.x == 0));

        if (isPawn && reachedEnd)
        {
            state        = GameState::Promotion;
            promotionPos = pathTarget;
        }
        else
        {
            turn = (turn == Color::White) ? Color::Black : Color::White;
        }

        selectedPiece = nullptr;
        validMoves.clear();
        pathSquares.clear();
        pathAnimating = false;
        pathTime      = 0.0f;
    }
}

void Game::clearPathAnimation()
{
    pathAnimating = false;
    pathTime      = 0.0f;
    pathSquares.clear();
}
