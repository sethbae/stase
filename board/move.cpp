#include <stdint.h>
#include <string>
using std::string;
#include "board.h"
#include "helper.h"
#include "print.h"

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
    bool is_cas() { return flags & 2; }
    bool is_ep() { return flags & 4; }
    bool is_cap() { return flags & 8; }

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
        return (flags & (1 << 6)) == 0;
    }

    int ep_file() {
        return get_x(to);
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

// Calculated by hand
// Least significat bit represents a1
static const uint64_t file_a = 72340172838076673L;      // a file
static const uint64_t rank_one = 255L;                  // first rank

static const uint64_t diag_nxy = 72624976668147840L;    // a8 to h1
static const uint64_t diag_xy = -9205322385119247871L;  // a1 to h8

uint64_t gen_file_mask(Byte file) { return file_a << file; }

uint64_t gen_row_mask(Byte row) { return rank_one << (row * 8); }

uint64_t gen_nxy_mask(Square sq) {
    uint64_t new_nxy = diag_nxy;
    int dif_nxy = get_y(sq) + get_x(sq) - 7;
    if (dif_nxy > 0) {
        new_nxy = diag_nxy << (dif_nxy * 8);
    } else if (dif_nxy < 0) {
        new_nxy = diag_nxy >> (-dif_nxy * 8);
    }

    return new_nxy;
}

uint64_t gen_xy_mask(Square sq) {
    uint64_t new_xy = diag_xy;
    int dif_xy = get_y(sq) - get_x(sq);
    if (dif_xy > 0) {
        new_xy = diag_xy << (dif_xy * 8);
        // new_xy = diag_xy << 8;
    } else if (dif_xy < 0) {
        new_xy = diag_xy >> (-dif_xy * 8);
    }
    return new_xy;
}

uint64_t gen_diags(Square sq) {
    return gen_nxy_mask(sq) | gen_xy_mask(sq);
}

uint64_t gen_ortho(Square sq) {
    return gen_file_mask(get_x(sq)) | gen_row_mask(get_y(sq));
}

uint64_t gen_knight_mask(Square sq) {
    return ((gen_row_mask(get_y(sq) + 2) | gen_row_mask(get_y(sq) - 2)) &
           (gen_file_mask(get_x(sq) + 1) | gen_file_mask(get_x(sq) - 1))) |
           ((gen_row_mask(get_y(sq) + 1) | gen_row_mask(get_y(sq) - 1)) & 
           (gen_file_mask(get_x(sq) + 2) | gen_file_mask(get_x(sq) - 2)));
}


// Result is undefined if the move is not on a diagonal or orthogonal line, eg. a knight's move
bool is_obstructed(const Board & b, Move m) {
    int x_offset = get_x(m.to) > get_x(m.from) ? 1 : get_x(m.to) < get_x(m.from) ? -1 : 0;
    int y_offset = get_y(m.to) > get_y(m.from) ? 1 : get_y(m.to) < get_y(m.from) ? -1 : 0;
    
    int x = get_x(m.from) + x_offset;
    int y = get_y(m.from) + y_offset;

    while (get_y(m.to) != y || get_x(m.to) != x) {
        if (TYPE[b.get(mksq(x, y))] != EMPTY) {
            return true;
        }

        x += x_offset;
        y += y_offset;
    }

    return false;
}

int main(void) {
    Board b = starting_pos();
    pr(b);
}

