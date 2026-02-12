#pragma once
#include <string>

enum class PieceType { None, Pawn, Rook, Knight, Bishop, Queen, King };
enum class PieceColor { None, White, Black };

class Piece {
public:
    PieceType type = PieceType::None;
    PieceColor color = PieceColor::None;

    // Constructeurs
    Piece() = default;
    Piece(PieceType t, PieceColor c);

    // Pour récupérer le caractère à afficher (ex: "K", "P")
    const char* getSymbol() const;
};