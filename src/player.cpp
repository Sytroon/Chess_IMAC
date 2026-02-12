#include "player.hpp"
#include <iostream>

Player::Player(bool isWhite, std::string playerName) 
    : name(playerName), isHuman(true), score(0) 
{
    // On définit la couleur en fonction du booléen
    if (isWhite) {
        color = PieceColor::White;
    } else {
        color = PieceColor::Black;
    }
}

std::string Player::getName() const {
    return name;
}

PieceColor Player::getColor() const {
    return color;
}

bool Player::isWhiteSide() const {
    return color == PieceColor::White;
}

void Player::printInfo() const {
    std::cout << "Joueur: " << name 
              << " | Couleur: " << (isWhiteSide() ? "Blanc" : "Noir") 
              << std::endl;
}