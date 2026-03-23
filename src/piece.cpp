#include "Piece.hpp"
#include "Board.hpp"

Knight::Knight(Color c, Position p)
    : Piece(c, p, (c == Color::Black ? "♘" : "♞")) {}

std::vector<Position> Knight::getPossibleMoves(const Board& board) const
{
    std::vector<Position> moves;
    int                   dx[] = {1, 2, 2, 1, -1, -2, -2, -1};
    int                   dy[] = {2, 1, -1, -2, -2, -1, 1, 2};

    for (int i = 0; i < 8; ++i)
    {
        Position target{pos.x + dx[i], pos.y + dy[i]};
        if (board.isInside(target))
        {
            Piece* p = board.getPiece(target);
            if (p == nullptr || p->getColor() != color)
            {
                moves.push_back(target);
            }
        }
    }
    return moves;
}

Pawn::Pawn(Color c, Position p) : Piece(c, p, (c == Color::Black ? "♙" : "♟")) {}
Rook::Rook(Color c, Position p) : Piece(c, p, (c == Color::Black ? "♖" : "♜")) {}
Bishop::Bishop(Color c, Position p) : Piece(c, p, (c == Color::Black ? "♗" : "♝")) {}
Queen::Queen(Color c, Position p) : Piece(c, p, (c == Color::Black ? "♕" : "♛")) {}
King::King(Color c, Position p) : Piece(c, p, (c == Color::Black ? "♔" : "♚")) {}

std::vector<Position> Pawn::getPossibleMoves(const Board& board) const
{
    std::vector<Position> moves;
    int                   direction = (color == Color::Black) ? 1 : -1;

    // Avancer d'une case
    Position forward{pos.x + direction, pos.y};
    if (board.isInside(forward) && !board.getPiece(forward))
    {
        moves.push_back(forward);
        // Premier mouvement : deux cases
        Position forward2{pos.x + 2 * direction, pos.y};
        bool     isStartPos = (color == Color::Black && pos.x == 1) || (color == Color::White && pos.x == 6);
        if (isStartPos && !board.getPiece(forward2))
            moves.push_back(forward2);
    }

    // Captures en diagonale
    int sideDirs[] = {-1, 1};
    for (int side : sideDirs)
    {
        Position diag{pos.x + direction, pos.y + side};
        if (board.isInside(diag))
        {
            Piece* target = board.getPiece(diag);
            if (target && target->getColor() != color)
                moves.push_back(diag);
        }
    }
    return moves;
}

void addSlidingMoves(std::vector<Position>& moves, Position pos, int dx, int dy, Color color, const Board& board)
{
    Position current = {pos.x + dx, pos.y + dy};
    while (board.isInside(current))
    {
        Piece* p = board.getPiece(current);
        if (!p)
        {
            moves.push_back(current);
        }
        else
        {
            if (p->getColor() != color)
                moves.push_back(current);
            break; // Obstacle rencontré
        }
        current.x += dx;
        current.y += dy;
    }
}

std::vector<Position> Rook::getPossibleMoves(const Board& board) const
{
    std::vector<Position> moves;
    int                   dx[] = {1, -1, 0, 0}, dy[] = {0, 0, 1, -1};
    for (int i = 0; i < 4; ++i)
        addSlidingMoves(moves, pos, dx[i], dy[i], color, board);
    return moves;
}

std::vector<Position> Bishop::getPossibleMoves(const Board& board) const
{
    std::vector<Position> moves;
    int                   dx[] = {1, 1, -1, -1}, dy[] = {1, -1, 1, -1};
    for (int i = 0; i < 4; ++i)
        addSlidingMoves(moves, pos, dx[i], dy[i], color, board);
    return moves;
}

std::vector<Position> Queen::getPossibleMoves(const Board& board) const
{
    // La reine est la combinaison de la Tour et du Fou
    auto m1 = Rook(color, pos).getPossibleMoves(board);
    auto m2 = Bishop(color, pos).getPossibleMoves(board);
    m1.insert(m1.end(), m2.begin(), m2.end());
    return m1;
}

std::vector<Position> King::getPossibleMoves(const Board& board) const
{
    std::vector<Position> moves;
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            if (dx == 0 && dy == 0)
                continue;
            Position target{pos.x + dx, pos.y + dy};
            if (board.isInside(target))
            {
                Piece* p = board.getPiece(target);
                if (!p || p->getColor() != color)
                    moves.push_back(target);
            }
        }
    }
    return moves;
}