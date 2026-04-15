#include "piece.hpp"
#include "board.hpp"

namespace {
    /**
     * @brief Helper function to compute sliding moves for Rooks, Bishops, and Queens.
     * @param moves The vector to append valid positions to.
     * @param pos The starting position of the piece.
     * @param dx The step direction on the X axis.
     * @param dy The step direction on the Y axis.
     * @param color The color of the moving piece (to prevent capturing own pieces).
     * @param board The current chess board.
     */
    void addSlidingMoves(std::vector<Position>& moves, Position pos, int dx, int dy, Color color, const Board& board) {
        Position current = {pos.x + dx, pos.y + dy};
        
        while (board.isInside(current)) {
            Piece* p = board.getPiece(current);
            if (!p) {
                // Empty square, valid move
                moves.push_back(current);
            } else {
                // Occupied square, check if it's an opponent's piece
                if (p->getColor() != color) {
                    moves.push_back(current); 
                }
                break; // Stop sliding if an obstacle is encountered
            }
            current.x += dx;
            current.y += dy;
        }
    }
} // namespace

// Pawn ---------------------------------------------------------------------------------

Pawn::Pawn(Color c, Position p) 
    : Piece(c, p, "♟") {}

std::vector<Position> Pawn::getPossibleMoves(const Board& board) const {
    std::vector<Position> moves;
    
    // Black moves +1 in X (down the board), White moves -1 in X (up the board)
    int direction = (color == Color::Black) ? 1 : -1;

    // 1. Move forward one square
    Position forward{pos.x + direction, pos.y};
    if (board.isInside(forward) && !board.getPiece(forward)) {
        moves.push_back(forward);
        
        // 2. First move: allow moving two squares forward
        Position forward2{pos.x + 2 * direction, pos.y};
        bool isStartPos = (color == Color::Black && pos.x == 1) || (color == Color::White && pos.x == 6);
        if (isStartPos && !board.getPiece(forward2)) {
            moves.push_back(forward2);
        }
    }

    // 3. Diagonal captures
    int sideDirs[] = {-1, 1};
    for (int side : sideDirs) {
        Position diag{pos.x + direction, pos.y + side};
        if (board.isInside(diag)) {
            Piece* target = board.getPiece(diag);
            if (target && target->getColor() != color) {
                moves.push_back(diag);
            }
        }
    }
    return moves;
}

// Knight ---------------------------------------------------------------------------------

Knight::Knight(Color c, Position p)
    : Piece(c, p, "♞") {}

std::vector<Position> Knight::getPossibleMoves(const Board& board) const {
    std::vector<Position> moves;
    
    // 8 possible L-shaped moves for a Knight
    int dx[] = {1, 2, 2, 1, -1, -2, -2, -1};
    int dy[] = {2, 1, -1, -2, -2, -1, 1, 2};

    for (int i = 0; i < 8; ++i) {
        Position target{pos.x + dx[i], pos.y + dy[i]};
        if (board.isInside(target)) {
            Piece* p = board.getPiece(target);
            if (p == nullptr || p->getColor() != color) {
                moves.push_back(target);
            }
        }
    }
    return moves;
}

// Bishop ---------------------------------------------------------------------------------

Bishop::Bishop(Color c, Position p) 
    : Piece(c, p, "♝") {}

std::vector<Position> Bishop::getPossibleMoves(const Board& board) const {
    std::vector<Position> moves;
    
    // 4 diagonal directions
    int dx[] = {1, 1, -1, -1};
    int dy[] = {1, -1, 1, -1};
    
    for (int i = 0; i < 4; ++i) {
        addSlidingMoves(moves, pos, dx[i], dy[i], color, board);
    }
    return moves;
}

// Rook ---------------------------------------------------------------------------------

Rook::Rook(Color c, Position p) 
    : Piece(c, p, "♜") {}

std::vector<Position> Rook::getPossibleMoves(const Board& board) const {
    std::vector<Position> moves;
    
    // 4 straight directions (horizontal and vertical)
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};
    
    for (int i = 0; i < 4; ++i) {
        addSlidingMoves(moves, pos, dx[i], dy[i], color, board);
    }
    return moves;
}

// Queen ---------------------------------------------------------------------------------

Queen::Queen(Color c, Position p) 
    : Piece(c, p, "♛") {}

std::vector<Position> Queen::getPossibleMoves(const Board& board) const {
    // A Queen's move set is simply the union of a Rook's and a Bishop's
    auto m1 = Rook(color, pos).getPossibleMoves(board);
    auto m2 = Bishop(color, pos).getPossibleMoves(board);
    
    m1.insert(m1.end(), m2.begin(), m2.end());
    return m1;
}

// King ---------------------------------------------------------------------------------

King::King(Color c, Position p) 
    : Piece(c, p, "♚") {}

std::vector<Position> King::getPossibleMoves(const Board& board) const {
    std::vector<Position> moves;
    
    // Immediate 8 neighboring squares
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue; // Skip current position
            
            Position target{pos.x + dx, pos.y + dy};
            if (board.isInside(target)) {
                Piece* p = board.getPiece(target);
                if (!p || p->getColor() != color) {
                    moves.push_back(target);
                }
            }
        }
    }
    return moves;
}