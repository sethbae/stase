#include <stdint.h>
#include <string>
using std::string;
#include "board.h"
#include "helper.h"

/* defines a move data type and defines functions to make moves, and return legal moves etc */

// [8 bits from] [8 bits to] [1 bit is_prom] [2 bits promotion piece] [13 bits: flags]


struct Move {
    uint_fast8_t from;
    uint_fast8_t to;
    uint_fast16_t flags;    
};

void make_move(Board & b, Move m) {
    // can't do castles or en-passant
    Piece p = b.get(m.from);
    b.set(m.to, p);
    b.set(m.from, EMPTY);

}

string ptos_alg(Piece p) {
    switch (TYPE[p]) {
        case KING: return "K";
        case QUEEN: return "Q";
        case ROOK: return "R";
        case BISHOP: return "B";
        case KNIGHT: return "N";
        case PAWN: return "";
        default: return "";
    }
}

Move santomove(string san) {
    // needs to be able to find where the piece is coming from given its destination
    Move m = {0, 0, 0};
    return m;
}

string movetosan(Board & b, Move m) {
    // doesn't do captures, checks, promotions or disambiguations
    return ptos_alg(b.get(m.from)) + sqtos(m.to);
}

