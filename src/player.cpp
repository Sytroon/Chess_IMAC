#include "player.hpp"
#include "piece.hpp"

Player::Player(bool color, std::string name)
    : color(color), name(name) {};

void Player::initializePieces()
{
    if (color)
    {
        pieces.emplace_back(color, "R", Position{.pos_x = 0, .pos_y = 0});
        pieces.emplace_back(color, "k", Position{.pos_x = 0, .pos_y = 1});
        pieces.emplace_back(color, "B", Position{.pos_x = 0, .pos_y = 2});
        pieces.emplace_back(color, "Q", Position{.pos_x = 0, .pos_y = 3});
        pieces.emplace_back(color, "K", Position{.pos_x = 0, .pos_y = 4});
        pieces.emplace_back(color, "B", Position{.pos_x = 0, .pos_y = 5});
        pieces.emplace_back(color, "k", Position{.pos_x = 0, .pos_y = 6});
        pieces.emplace_back(color, "R", Position{.pos_x = 0, .pos_y = 7});

        pieces.emplace_back(color, "p", Position{.pos_x = 1, .pos_y = 0});
        pieces.emplace_back(color, "p", Position{.pos_x = 1, .pos_y = 1});
        pieces.emplace_back(color, "p", Position{.pos_x = 1, .pos_y = 2});
        pieces.emplace_back(color, "p", Position{.pos_x = 1, .pos_y = 3});
        pieces.emplace_back(color, "p", Position{.pos_x = 1, .pos_y = 4});
        pieces.emplace_back(color, "p", Position{.pos_x = 1, .pos_y = 5});
        pieces.emplace_back(color, "p", Position{.pos_x = 1, .pos_y = 6});
        pieces.emplace_back(color, "p", Position{.pos_x = 1, .pos_y = 7});
    }
    else
    {
        pieces.emplace_back(color, "R", Position{.pos_x = 7, .pos_y = 0});
        pieces.emplace_back(color, "k", Position{.pos_x = 7, .pos_y = 1});
        pieces.emplace_back(color, "B", Position{.pos_x = 7, .pos_y = 2});
        pieces.emplace_back(color, "Q", Position{.pos_x = 7, .pos_y = 3});
        pieces.emplace_back(color, "K", Position{.pos_x = 7, .pos_y = 4});
        pieces.emplace_back(color, "B", Position{.pos_x = 7, .pos_y = 5});
        pieces.emplace_back(color, "k", Position{.pos_x = 7, .pos_y = 6});
        pieces.emplace_back(color, "R", Position{.pos_x = 7, .pos_y = 7});

        pieces.emplace_back(color, "p", Position{.pos_x = 6, .pos_y = 0});
        pieces.emplace_back(color, "p", Position{.pos_x = 6, .pos_y = 1});
        pieces.emplace_back(color, "p", Position{.pos_x = 6, .pos_y = 2});
        pieces.emplace_back(color, "p", Position{.pos_x = 6, .pos_y = 3});
        pieces.emplace_back(color, "p", Position{.pos_x = 6, .pos_y = 4});
        pieces.emplace_back(color, "p", Position{.pos_x = 6, .pos_y = 5});
        pieces.emplace_back(color, "p", Position{.pos_x = 6, .pos_y = 6});
        pieces.emplace_back(color, "p", Position{.pos_x = 6, .pos_y = 7});
    }
};

Position Player::play(Position pos)
{
}