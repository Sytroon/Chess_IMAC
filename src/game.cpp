#include "Game.hpp"

void Game::handleSquareClick(Position p)
{
    if (state != GameState::Playing)
        return; // Bloque le jeu si fini

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

                // 2. Déplacement de la pièce
                board.movePiece(selectedPiece->getPos(), p);

                // 3. DETECTION PROMOTION
                Piece* movedPiece = board.getPiece(p);
                bool   isPawn     = (movedPiece->getIcon() == "\u2659" || movedPiece->getIcon() == "\u265F");
                bool   reachedEnd = (movedPiece->getColor() == Color::Black && p.x == 7) || (movedPiece->getColor() == Color::White && p.x == 0);

                if (isPawn && reachedEnd)
                {
                    state        = GameState::Promotion;
                    promotionPos = p;
                    // On ne change pas le tour tout de suite !
                    // On attend que la promotion soit choisie.
                }
                else
                {
                    turn = (turn == Color::White) ? Color::Black : Color::White;
                }

                selectedPiece = nullptr;
                validMoves.clear();
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