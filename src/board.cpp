#include "board.hpp"
#include <iostream>
#include "quick_imgui/quick_imgui.hpp"

Board::Board()
{
    initialize();
}

void Board::initialize()
{
    // Vider le plateau
    for (auto& p : squares)
        p = Piece();

    // Placer les Pions (Ligne 1 pour Blancs, Ligne 6 pour Noirs)
    for (int i = 0; i < 8; ++i)
    {
        squares[1 * 8 + i] = Piece(PieceType::Pawn, PieceColor::Black); // Ligne index 1
        squares[6 * 8 + i] = Piece(PieceType::Pawn, PieceColor::White); // Ligne index 6
    }

    // Placer les autres pièces
    squares[0]  = Piece(PieceType::Rook, PieceColor::Black);
    squares[7]  = Piece(PieceType::Rook, PieceColor::Black);
    squares[56] = Piece(PieceType::Rook, PieceColor::White);
    squares[63] = Piece(PieceType::Rook, PieceColor::White);

    squares[1]  = Piece(PieceType::Knight, PieceColor::Black);
    squares[6]  = Piece(PieceType::Knight, PieceColor::Black);
    squares[57] = Piece(PieceType::Knight, PieceColor::White);
    squares[62] = Piece(PieceType::Knight, PieceColor::White);

    squares[2]  = Piece(PieceType::Bishop, PieceColor::Black);
    squares[5]  = Piece(PieceType::Bishop, PieceColor::Black);
    squares[58] = Piece(PieceType::Bishop, PieceColor::White);
    squares[61] = Piece(PieceType::Bishop, PieceColor::White);

    squares[3]  = Piece(PieceType::Queen, PieceColor::Black);
    squares[59] = Piece(PieceType::Queen, PieceColor::White);

    squares[4]  = Piece(PieceType::King, PieceColor::Black);
    squares[60] = Piece(PieceType::King, PieceColor::White);
}

// void Board::movePiece(int from, int to) {
//     // Copie la pièce vers la destination
//     squares[to] = squares[from];
//     // Vide la case de départ
//     squares[from] = Piece();
// }

// Directions orthogonales (Tour)
const std::vector<Offset> straightDirs = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
// Directions diagonales (Fou)
const std::vector<Offset> diagonalDirs = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
// Sauts du Cavalier
const std::vector<Offset> knightJump = {
    {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}
};
// Déplacements du Roi (les 8 cases autour)
const std::vector<Offset> kingMoves = {
    {0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
};

bool Board::tryAddMove(int originIndex, int targetX, int targetY)
{
    // 1. Vérif Limites du plateau
    if (targetX < 0 || targetX > 7 || targetY < 0 || targetY > 7)
        return false;

    int   targetIndex = targetY * 8 + targetX;
    Piece p           = squares[originIndex];
    Piece target      = squares[targetIndex];

    // 2. Case vide : on ajoute et on continue (return true)
    if (target.type == PieceType::None)
    {
        possibleMoves.push_back(targetIndex);
        return true;
    }

    // 3. Pièce ennemie : on mange (ajout) et on s'arrête (return false)
    if (target.color != p.color)
    {
        possibleMoves.push_back(targetIndex);
        return false;
    }

    // 4. Pièce amie : on est bloqué (pas d'ajout) et on s'arrête (return false)
    return false;
}

void Board::generateSlidingMoves(int index, const std::vector<Offset>& directions)
{
    int startX = index % 8;
    int startY = index / 8;

    for (const auto& dir : directions)
    {
        for (int dist = 1; dist < 8; ++dist)
        {
            int targetX = startX + (dir.x * dist);
            int targetY = startY + (dir.y * dist);

            // Si tryAddMove renvoie false (bloqué ou prise), on arrête cette direction
            if (!tryAddMove(index, targetX, targetY))
                break;
        }
    }
}

void Board::generateSteppingMoves(int index, const std::vector<Offset>& offsets)
{
    int startX = index % 8;
    int startY = index / 8;

    for (const auto& off : offsets)
    {
        // Pour le cavalier/roi, pas de boucle de distance, juste +offset
        tryAddMove(index, startX + off.x, startY + off.y);
    }
}

void Board::generatePawnMoves(int index)
{
    Piece p = squares[index];
    int   x = index % 8;
    int   y = index / 8;

    // Direction: -1 pour Blanc (monte), +1 pour Noir (descend)
    int dy       = (p.color == PieceColor::White) ? -1 : 1;
    int startRow = (p.color == PieceColor::White) ? 6 : 1;

    // 1. Avancer de 1 (Doit être vide)
    int forwardIndex = (y + dy) * 8 + x;
    if (y + dy >= 0 && y + dy <= 7 && squares[forwardIndex].type == PieceType::None)
    {
        possibleMoves.push_back(forwardIndex);

        // 2. Avancer de 2 (Uniquement si 1er tour et case intermédiaire vide)
        int doubleForwardIndex = (y + 2 * dy) * 8 + x;
        if (y == startRow && squares[doubleForwardIndex].type == PieceType::None)
        {
            possibleMoves.push_back(doubleForwardIndex);
        }
    }

    // 3. Manger en diagonale
    int captureOffsets[] = {-1, 1};
    for (int dx : captureOffsets)
    {
        int targetX = x + dx;
        int targetY = y + dy;

        if (targetX >= 0 && targetX <= 7 && targetY >= 0 && targetY <= 7)
        {
            int   targetIndex = targetY * 8 + targetX;
            Piece target      = squares[targetIndex];
            // On ne peut aller en diagonale QUE s'il y a un ennemi
            if (target.type != PieceType::None && target.color != p.color)
            {
                possibleMoves.push_back(targetIndex);
            }
        }
    }
}

void Board::calculatePossibleMoves(int index)
{
    possibleMoves.clear();
    Piece p = squares[index];

    switch (p.type)
    {
    case PieceType::Rook:
        generateSlidingMoves(index, straightDirs);
        break;

    case PieceType::Bishop:
        generateSlidingMoves(index, diagonalDirs);
        break;

    case PieceType::Queen:
        // La reine combine Tour + Fou
        generateSlidingMoves(index, straightDirs);
        generateSlidingMoves(index, diagonalDirs);
        break;

    case PieceType::Knight:
        generateSteppingMoves(index, knightJump);
        break;

    case PieceType::King:
        generateSteppingMoves(index, kingMoves);
        break;

    case PieceType::Pawn:
        generatePawnMoves(index);
        break;

    default:
        break;
    }
}

void Board::movePiece(int from, int to)
{
    // Sécurité : on vérifie si le mouvement est dans la liste des possibles
    // (Optionnel mais recommandé pour éviter la triche)
    bool isValid = false;
    for (int move : possibleMoves)
    {
        if (move == to)
        {
            isValid = true;
            break;
        }
    }

    if (isValid)
    {
        squares[to]   = squares[from];
        squares[from] = Piece(); // Vide l'ancienne case
    }

    // Après un mouvement, on désélectionne tout
    possibleMoves.clear();
    selectedSquareIndex = -1;
}

void Board::draw()
{
    ImVec2 buttonSize(50.f, 50.f);

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            int index = y * 8 + x;
            ImGui::PushID(index);

            // --- GESTION DES COULEURS ---
            bool   isWhiteSquare = (x + y) % 2 == 0;
            ImVec4 baseColor     = isWhiteSquare ? ImVec4(0.8f, 0.8f, 0.8f, 1.f) : ImVec4(0.4f, 0.4f, 0.4f, 1.f);

            // Vérifier si cette case est un mouvement possible
            bool isPossibleMove = false;
            for (int move : possibleMoves)
            {
                if (move == index)
                {
                    isPossibleMove = true;
                    break;
                }
            }

            if (selectedSquareIndex == index)
            {
                // Case sélectionnée : VERT
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 1.f, 0.f, 1.f));
            }
            else if (isPossibleMove)
            {
                // Case accessible : CYAN (ou rouge si c'est une attaque ?)
                if (squares[index].type != PieceType::None)                               // Attaque
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.5f, 0.5f, 1.f)); // Rouge clair
                else
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.8f, 0.8f, 1.f)); // Cyan
            }
            else
            {
                // Case normale
                ImGui::PushStyleColor(ImGuiCol_Button, baseColor);
            }

            // --- INTERACTION ---
            if (ImGui::Button(squares[index].getSymbol(), buttonSize))
            {
                if (selectedSquareIndex == -1)
                {
                    // Sélectionner une pièce
                    if (squares[index].type != PieceType::None)
                    {
                        selectedSquareIndex = index;
                        calculatePossibleMoves(index); // <--- CALCULER LES COUPS ICI
                    }
                }
                else
                {
                    // Si on clique sur la même case, on désélectionne
                    if (index == selectedSquareIndex)
                    {
                        selectedSquareIndex = -1;
                        possibleMoves.clear();
                    }
                    // Si on clique sur un mouvement possible -> on bouge
                    else if (isPossibleMove)
                    {
                        movePiece(selectedSquareIndex, index);
                    }
                    // Si on clique ailleurs (invalide), on change la sélection
                    else if (squares[index].color == squares[selectedSquareIndex].color)
                    {
                        selectedSquareIndex = index;
                        calculatePossibleMoves(index);
                    }
                    else
                    {
                        // Clic dans le vide invalide : on désélectionne
                        selectedSquareIndex = -1;
                        possibleMoves.clear();
                    }
                }
            }

            ImGui::PopStyleColor();
            ImGui::PopID();

            if (x < 7)
                ImGui::SameLine();
        }
    }
}