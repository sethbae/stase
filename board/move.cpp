#include <stdint.h>
#include <string>
using std::string;
#include "board.h"
#include "helper.h"

/* defines a move data type and defines functions to make moves, and return legal moves etc */

/* 
from (8 bits)
to (8 bits)
flags (16 bits):
    - bit 0:        is_prom
    - bit 1:        is_castle
    - bit 2:        is_enpassant
    - bit 3:        is_capture

    - bit 4-5:      promotion piece
    - bit 6:        castle side
    - bit 7-9:      captured piece
 */

struct Move {
    uint_fast8_t from;
    uint_fast8_t to;
    uint_fast16_t flags;    

    bool is_prom() { return flags & 1; }
    bool is_cas() { return flags & 2 != 0; }
    bool is_en() { return flags & 4 != 0; }
    bool is_cap() { return flags & 8 != 0; }

    Piece prom() {
        switch((flags & 6) >> 1) {
            case 1: return BISHOP;
            case 2: return KNIGHT;
            case 3: return ROOK;
            case 0:
            default: return QUEEN;
        }
    }


    int cas_side() {
        return flags & (1 << 6) == 0;
    }

    int en_file() {
        return get_col(to);
    }

    Piece cap_piece() {
        return (Piece) flags & (7 << 7);
    }

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

bool legal(Move m) {
    // checks if a move is legal
    return true;
}
