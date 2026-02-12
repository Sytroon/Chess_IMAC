#include "piece.hpp"

Piece::Piece(PieceType t, PieceColor c) : type(t), color(c) {}

const char* Piece::getSymbol() const {
    if (color == PieceColor::None) return " ";
    
    // Exemple simple. Tu pourras mettre des icônes UTF-8 plus tard
    switch (type) {
        case PieceType::Pawn:   return (color == PieceColor::White) ? "P" : "p";
        case PieceType::Rook:   return (color == PieceColor::White) ? "R" : "r";
        case PieceType::Knight: return (color == PieceColor::White) ? "N" : "n";
        case PieceType::Bishop: return (color == PieceColor::White) ? "B" : "b";
        case PieceType::Queen:  return (color == PieceColor::White) ? "Q" : "q";
        case PieceType::King:   return (color == PieceColor::White) ? "K" : "k";
        default: return " ";
    }
}