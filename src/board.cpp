#include "Board.hpp"
#include "piece.hpp"

Board::Board()
{
    reset();
}

void Board::reset()
{
    for (auto& row : grid)
        for (auto& cell : row)
            cell.reset();

    for (int i = 0; i < 8; ++i)
    {
        grid[1][i] = std::make_unique<Pawn>(Color::Black, Position{1, i});
        grid[6][i] = std::make_unique<Pawn>(Color::White, Position{6, i});
    }
    // Remplis ici les Tours (0,0 / 0,7), Cavaliers, Fous, Reine (0,3) et Roi (0,4)...
    grid[0][0] = std::make_unique<Rook>(Color::Black, Position{0, 0});
    grid[0][7] = std::make_unique<Rook>(Color::Black, Position{0, 7});
    grid[0][1] = std::make_unique<Knight>(Color::Black, Position{0, 1});
    grid[0][6] = std::make_unique<Knight>(Color::Black, Position{0, 6});
    grid[0][2] = std::make_unique<Bishop>(Color::Black, Position{0, 2});
    grid[0][5] = std::make_unique<Bishop>(Color::Black, Position{0, 5});
    grid[0][3] = std::make_unique<Queen>(Color::Black, Position{0, 3});
    grid[0][4] = std::make_unique<King>(Color::Black, Position{0, 4});

    grid[7][0] = std::make_unique<Rook>(Color::White, Position{7, 0});
    grid[7][7] = std::make_unique<Rook>(Color::White, Position{7, 7});
    grid[7][1] = std::make_unique<Knight>(Color::White, Position{7, 1});
    grid[7][6] = std::make_unique<Knight>(Color::White, Position{7, 6});
    grid[7][2] = std::make_unique<Bishop>(Color::White, Position{7, 2});
    grid[7][5] = std::make_unique<Bishop>(Color::White, Position{7, 5});
    grid[7][3] = std::make_unique<Queen>(Color::White, Position{7, 3});
    grid[7][4] = std::make_unique<King>(Color::White, Position{7, 4});
}

Piece* Board::getPiece(Position p) const
{
    return grid[p.x][p.y].get();
}

void Board::movePiece(Position from, Position to)
{
    if (grid[from.x][from.y])
    {
        grid[to.x][to.y] = std::move(grid[from.x][from.y]);
        grid[to.x][to.y]->setPos(to);
    }
}

void Board::setPiece(Position p, std::unique_ptr<Piece> newPiece)
{
    if (isInside(p))
    {
        grid[p.x][p.y] = std::move(newPiece);
    }
}