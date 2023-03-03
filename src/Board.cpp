#include "Board.hpp"
using namespace ce;

Board::Board(){
    _board[0] = BROOK;
    _board[1] = BKNIGHT;
    _board[2] = BBISHOP;
    _board[3] = BQUEEN;
    _board[4] = BKING;
    _board[5] = BBISHOP;
    _board[6] = BKNIGHT;
    _board[7] = BROOK;
    for(uchar i = 8;i < 16;i++) _board[i] = BPAWN;
    for(uchar i = 0;i < 255;i++){
        file[i] = i & 7;
        rank[i] = i >> 3;
    }
    for(uchar i = 0;i < 16;i++){
        _board[(7-rank[i]<<3)|file[i]] = Piece(-_board[i]);
    }
    _turn = 0;
    _wking = 60;
    _bking = 4;
    for(uchar i = 0;i < 64;i++){
        uchar j = i;
        while(file[j] && rank[j]){
            j -= 9;
            diag[0][i].push_back(j);
        }
        j = i;
        while(file[j] < 7 && rank[j]){
            j -= 7;
            diag[1][i].push_back(j);
        }
        j = i;
        while(file[j] < 7 && rank[j] < 7){
            j += 9;
            diag[2][i].push_back(j);
        }
        j = i;
        while(file[j] && rank[j] < 7){
            j += 7;
            diag[3][i].push_back(j);
        }
        j = i;
        while(rank[j]){
            j -= 8;
            axis[0][i].push_back(j);
        }
        j = i;
        while(file[j] < 7){
            j++;
            axis[1][i].push_back(j);
        }
        j = i;
        while(rank[j] < 7){
            j += 8;
            axis[2][i].push_back(j);
        }
        j = i;
        while(file[j]){
            j--;
            axis[3][i].push_back(j);
        }
        if(file[i]) king[i].push_back(i-1);
        if(file[i] && rank[i]) king[i].push_back(i-9);
        if(rank[i]) king[i].push_back(i-8);
        if(rank[i] && file[i] < 7) king[i].push_back(i-7);
        if(file[i] < 7) king[i].push_back(i+1);
        if(file[i] < 7 && rank[i] < 7) king[i].push_back(i+9);
        if(rank[i] < 7) king[i].push_back(i+8);
        if(file[i] && rank[i] < 7) king[i].push_back(i+7);
        for(char df = -2;df < 3;df++){
            if(!df) continue;
            for(char dr = -2; dr < 3;dr++){
                if(!dr) continue;
                char nf = file[i]+df;
                char nr = rank[i]+dr;
                if((dr+df&1) && 0 <= nf && nf < 8 && 0 <= nr && nr < 8) knight[i].push_back(nr<<3|nf);
            }
        }
    }
}

void Board::makeMove(Move move){
    UndoMove undo;
    undo.move = move;
    if(move.target > 63){
        _board[move.source] = Piece((move.target-64-file[move.target]-(_turn*7<<3)>>3) * (_turn ? -1 : 1));
        std::cout << int(move.target) << '\n';
        move.target = file[move.target]+(_turn*7<<3);
        std::cout << int(move.target) << '\n';
        undo.move.target = move.target+64;
    }
    undo.capturedPiece = _board[move.target];
    undo.enPassantPawn = _enPassantPawn;
    undo.leftRook = _rookMove[1<<_turn];
    undo.rightRook = _rookMove[1<<_turn|1];
    _lastMoveUndo.push(undo);
    if(_board[move.source] == Piece::BPAWN || _board[move.source] == Piece::WPAWN){
        if(move.target == move.source + (_turn ? 16 : -16)) _enPassantPawn = move.target;
        else _enPassantPawn = 65;
        if(_board[move.target] == Piece::NONE && file[move.target] != file[move.source]) {
            _board[move.target-(_turn ? 8 : -8)] = Piece::NONE;
        }
    } else _enPassantPawn = 65;
    if(_board[move.source] == Piece::BKING){
        _bking = move.target;
        _rookMove[2] = _rookMove[3] = true;
        if(move.target == 2 + move.source) std::swap(_board[7], _board[5]);
        if(move.target+2 == move.source) std::swap(_board[0], _board[3]);
    } else if (_board[move.source] == Piece::WKING){
        _wking = move.target;
        _rookMove[0] = _rookMove[1] = true;
        if(move.target == 2 + move.source) std::swap(_board[63], _board[61]);
        if(move.target+2 == move.source) std::swap(_board[56], _board[59]);
    }
    if(!move.target || !move.source) _rookMove[2] = true;
    if(move.target == 7 || move.source == 7) _rookMove[3] = true;
    if(move.target == 56 || move.source == 56) _rookMove[0] = true;
    if(move.target == 63 || move.source == 63) _rookMove[1] = true;
    _board[move.target] = _board[move.source];
    _board[move.source] = Piece::NONE;
    _turn ^= 1;
}

void Board::undoLastMove(){
    _turn ^= 1;
    UndoMove undo = _lastMoveUndo.top();
    _lastMoveUndo.pop();
    _enPassantPawn = undo.enPassantPawn;
    _rookMove[1<<_turn] = undo.leftRook;
    _rookMove[1<<_turn|1] = undo.rightRook;
    if(undo.move.target > 63){
        undo.move.target -= 64;
        _board[undo.move.target] = _turn ? Piece::BPAWN : Piece::WPAWN;
    } else if(_board[undo.move.target] == WKING || _board[undo.move.target] == BKING){
        if(_turn) _bking = undo.move.source;
        else _wking = undo.move.source;
        if(undo.move.target+2 == undo.move.source) std::swap(_board[undo.move.target+1], _board[undo.move.target-2]);
        if(undo.move.target-2 == undo.move.source) std::swap(_board[undo.move.target-1], _board[undo.move.target+1]);
    } else if ((_board[undo.move.target] == WPAWN || _board[undo.move.target] == BPAWN)
                && undo.capturedPiece == Piece::NONE && file[undo.move.target] != file[undo.move.source]) {
        if(_turn) _board[undo.move.target-8] = Piece::WPAWN;
        else _board[undo.move.target+8] = Piece::BPAWN;
    }
    _board[undo.move.source] = _board[undo.move.target];
    _board[undo.move.target] = undo.capturedPiece;
}

bool Board::inCheck(uchar pos) const {
    int s = _board[pos] > 0 ? 1 : -1;
    for(uchar j : knight[pos]) if(_board[j] == s*BKNIGHT) return true;
    for(uchar j : king[pos]) if(_board[j] == s*BKING) return true;
    for(uchar i = 0;i < 4;i++) {
        for(uchar j : diag[i][pos]){
            if(_board[j] == s*BBISHOP || _board[j] == s*BQUEEN) return true;
            if(_board[j]) break;
        }
    }
    for(int i = 0;i < 4;i++){
        for(uchar j : axis[i][pos]){
            if(_board[j] == s*BROOK || _board[j] == s*BQUEEN) return true;
            if(_board[j]) break;
        }
    }
    if(s == 1) return rank[pos] && ((file[pos] && _board[pos-9] == BPAWN) || (file[pos] < 7 && _board[pos-7] == BPAWN));
    return rank[pos]<7 && ((file[pos] && _board[pos+7] == WPAWN) || (file[pos] < 7 && _board[pos+9] == WPAWN));
}

void Board::getMoves(uchar pos, std::vector<Move>& legalMoves){
    std::vector<Move> moves;
    if(_board[pos] > 0){
        if(_board[pos] == WPAWN){
            if(!_board[pos-8]) {
                moves.emplace_back(pos, pos-8);
                if(rank[pos] == 6 && !_board[pos-16]) moves.emplace_back(pos, pos-16);
            }
            if(rank[pos] == 3 && file[pos] && _enPassantPawn == pos-1) moves.emplace_back(pos, pos-9);
            if(rank[pos] == 3 && file[pos] < 7 && _enPassantPawn == pos+1) moves.emplace_back(pos, pos-7);
            if(file[pos] && _board[pos-9] < 0) moves.emplace_back(pos, pos-9);
            if(file[pos] < 7 && _board[pos-7] < 0) moves.emplace_back(pos, pos-7);
            if(rank[pos] == 1) {
                int sz = moves.size();
                for(int i = 0;i < sz;i++){
                    moves[i].target |= 64;
                    moves.emplace_back(moves[i].source, moves[i].target+8*WKNIGHT);
                    moves.emplace_back(moves[i].source, moves[i].target+8*WBISHOP);
                    moves.emplace_back(moves[i].source, moves[i].target+8*WROOK);
                    moves[i].target += 8*WQUEEN;
                }
            }
        } else if (_board[pos] == WKNIGHT){
            for(uchar j : knight[pos]) if(_board[j] <= 0) moves.emplace_back(pos, j);
        } else if(_board[pos] == WKING){
            for(uchar j : king[pos]) if(_board[j] <= 0) moves.emplace_back(pos, j);
            if(!_rookMove[0] && !(_board[pos-1]|_board[pos-2]|_board[pos-3])) moves.emplace_back(pos, pos-2);
            if(!_rookMove[1] && !(_board[pos+1]|_board[pos+2])) moves.emplace_back(pos, pos+2);
        } else {
            if(_board[pos] == WBISHOP || _board[pos] == WQUEEN){
                for(uchar i = 0;i < 4;i++) for(uchar j : diag[i][pos]) {
                    if(_board[j] > 0) break;
                    moves.emplace_back(pos, j);
                    if(_board[j]) break;
                }
            }
            if(_board[pos] == WROOK || _board[pos] == WQUEEN){
                for(uchar i = 0;i < 4;i++) for(uchar j : axis[i][pos]) {
                    if(_board[j] > 0) break;
                    moves.emplace_back(pos, j);
                    if(_board[j]) break;
                }
            }
        }
    } else {
        if(_board[pos] == BPAWN){
            if(!_board[pos+8]) {
                moves.emplace_back(pos, pos+8);
                if(rank[pos] == 1 && !_board[pos+16]) moves.emplace_back(pos, pos+16);
            }
            if(rank[pos] == 4 && file[pos] && _enPassantPawn == pos-1) moves.emplace_back(pos, pos+7);
            if(rank[pos] == 4 && file[pos] < 7 && _enPassantPawn == pos+1) moves.emplace_back(pos, pos+9);
            if(file[pos] < 7 && _board[pos+9] > 0) moves.emplace_back(pos, pos+9);
            if(file[pos] && _board[pos+7] > 0) moves.emplace_back(pos, pos+7);
            if(rank[pos] == 6) {
                int sz = moves.size();
                for(int i = 0;i < sz;i++){
                    moves[i].target |= 64;
                    moves.emplace_back(moves[i].source, moves[i].target+8*WKNIGHT);
                    moves.emplace_back(moves[i].source, moves[i].target+8*WBISHOP);
                    moves.emplace_back(moves[i].source, moves[i].target+8*WROOK);
                    moves[i].target += 8*WQUEEN;
                }
            }
        } else if (_board[pos] == BKNIGHT){
            for(uchar j : knight[pos]) if(_board[j] >= 0) moves.emplace_back(pos, j);
        } else if(_board[pos] == BKING){
            for(uchar j : king[pos]) if(_board[j] >= 0) moves.emplace_back(pos, j);
            if(!_rookMove[2] && !(_board[pos-1]|_board[pos-2]|_board[pos-3])) moves.emplace_back(pos, pos-2);
            if(!_rookMove[3] && !(_board[pos+1]|_board[pos+2])) moves.emplace_back(pos, pos+2);
        } else {
            if(_board[pos] == BBISHOP || _board[pos] == BQUEEN){
                for(uchar i = 0;i < 4;i++) for(uchar j : diag[i][pos]) {
                    if(_board[j] < 0) break;
                    moves.emplace_back(pos, j);
                    if(_board[j]) break;
                }
            }
            if(_board[pos] == BROOK || _board[pos] == BQUEEN){
                for(uchar i = 0;i < 4;i++) for(uchar j : axis[i][pos]) {
                    if(_board[j] < 0) break;
                    moves.emplace_back(pos, j);
                    if(_board[j]) break;
                }
            }
        }
    }
    bool white = _board[pos] > 0;
    for(Move move : moves){
        makeMove(move);
        if(!inCheck(white ? _wking : _bking)) legalMoves.push_back(move);
        undoLastMove();
    }
}
Piece Board::getPiece(uchar rank, uchar file) const{
    return _board[rank<<3|file];
}
bool Board::getTurn() const {
    return _turn;
}