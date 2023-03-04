#ifndef __BOARD_HPP__
#define __BOARD_HPP__

#include <utility>
#include <stack>
#include <string>
#include <vector>

namespace ce {

using uchar = unsigned char;

enum Piece {
    BKING = -127,
    BQUEEN = -10,
    BROOK = -5,
    BBISHOP = -4,
    BKNIGHT = -3,
    BPAWN = -1,
    NONE = 0,
    WPAWN = 1,
    WKNIGHT = 3,
    WBISHOP = 4,
    WROOK = 5,
    WQUEEN = 10,
    WKING = 127,
};
struct Move{
    uchar source;
    uchar target;
    Move() : source(255), target(255) {}
    Move(uchar from, uchar to) : source(from), target(to) {}
};
struct UndoMove{
    Move move;
    Piece capturedPiece;
    int enPassantPawn;
    char rookMove = 0;
};
// En Passant pawn refers to the position of the pawn that moved 2 squares in the last move, or 64 if no such pawn did
class Board {
public:
    Board();
    Board(const std::string& fen);
    void makeMove(Move move);
    void undoLastMove();
    bool inCheck(uchar pos, int s) const;
    void getMoves(uchar pos, std::vector<Move>& legalMoves);
    Piece getPiece(uchar rank, uchar file) const;
    bool getTurn() const;
private:
    /*
    diag0: ^\ diag1: /^ diag2: \v diag3: v/
    axis0: ^ axis1: > axis2: v axis3: <
    */
    bool _turn;
    std::vector<uchar> diag[4][64], axis[4][64], king[64], knight[64];
    uchar file[256], rank[256];
    Piece _board[64] = {{Piece::NONE}};
    bool _rookMove[4] = {false};
    std::stack<UndoMove> _lastMoveUndo;
    uchar _enPassantPawn = 64;
    uchar _wking, _bking;
};

}

#endif