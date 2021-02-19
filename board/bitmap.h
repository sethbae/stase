#ifndef BITMAP_H
#define BITMAP_H

#include "board.h"
#include "helper.h"

typedef uint64_t Bitmap;

// Calculated by hand
// Least significat bit represents a1
static const Bitmap file_a = 72340172838076673L;      // a file
static const Bitmap rank_one = 255L;                  // first rank

static const Bitmap diag_nxy = 72624976668147840L;    // a8 to h1
static const Bitmap diag_xy = -9205322385119247871L;  // a1 to h8

Bitmap gen_pos(Square sq) {
    return ((Bitmap) 1) << get_x(sq) << (get_y(sq) * 8);
}

Bitmap gen_file(Byte file) { return file_a << file; }

Bitmap gen_row(Byte row) { return rank_one << (row * 8); }

Bitmap gen_nxy(Square sq) {
    Bitmap new_nxy = diag_nxy;
    int dif_nxy = get_y(sq) + get_x(sq) - 7;
    if (dif_nxy > 0) {
        new_nxy = diag_nxy << (dif_nxy * 8);
    } else if (dif_nxy < 0) {
        new_nxy = diag_nxy >> (-dif_nxy * 8);
    }

    return new_nxy;
}

Bitmap gen_xy(Square sq) {
    Bitmap new_xy = diag_xy;
    int dif_xy = get_y(sq) - get_x(sq);
    if (dif_xy > 0) {
        new_xy = diag_xy << (dif_xy * 8);
        // new_xy = diag_xy << 8;
    } else if (dif_xy < 0) {
        new_xy = diag_xy >> (-dif_xy * 8);
    }
    return new_xy;
}

Bitmap gen_diags(Square sq) {
    return gen_nxy(sq) | gen_xy(sq);
}

Bitmap gen_ortho(Square sq) {
    return gen_file(get_x(sq)) | gen_row(get_y(sq));
}

// inclusive
Bitmap gen_mcol(int from, int to) {
    return (255 >> (7 - (to - from)) << from) * file_a;
}

// inclusive too
Bitmap gen_mrow(int from, int to) {
    return (~((Bitmap) 0)) >> ((7 - (to - from)) * 8) << (from * 8);
}

Bitmap gen_knights(Square sq) {
    Bitmap queenmove = gen_ortho(sq) | gen_diags(sq);

    int minx = get_x(sq) < 2 ? 0 : get_x(sq) - 2;
    int miny = get_y(sq) < 2 ? 0 : get_y(sq) - 2;
    int maxx = get_x(sq) > 5 ? 7 : get_x(sq) + 2;
    int maxy = get_y(sq) > 5 ? 7 : get_y(sq) + 2;

    return ~queenmove & gen_mcol(minx, maxx) & gen_mrow(miny, maxy);
}

Bitmap gen_kings(Square sq) {
    int minx = get_x(sq) < 1 ? 0 : get_x(sq) - 1;
    int miny = get_y(sq) < 1 ? 0 : get_y(sq) - 1;
    int maxx = get_x(sq) > 6 ? 7 : get_x(sq) + 1;
    int maxy = get_y(sq) > 6 ? 7 : get_y(sq) + 1;

    return gen_mcol(minx, maxx) & gen_mrow(miny, maxy);
}

Bitmap gen_wpawn_attack(Square sq) {
    inc_y(sq);
    Bitmap map = (Bitmap) 0;

    Square temp = sq;
    if (get_x(sq) > 0) {
        dec_x(temp);
        map |= gen_pos(temp);
    }

    temp = sq;
    if (get_x(sq) < 7) {
        inc_x(temp);
        map |= gen_pos(temp);
    }

    return map;

}

Bitmap gen_wpawn_move(Square sq) {
    inc_y(sq);
    Bitmap map = gen_pos(sq);

    inc_y(sq);
    map |= gen_pos(sq);

    return map;
}


Bitmap gen_bpawn_attack(Square sq) {
    dec_y(sq);
    Bitmap map = (Bitmap) 0;

    Square temp = sq;
    if (get_x(sq) > 0) {
        dec_x(temp);
        map |= gen_pos(temp);
    }

    temp = sq;
    if (get_x(sq) < 7) {
        inc_x(temp);
        map |= gen_pos(temp);
    }

    return map;

}

Bitmap gen_bpawn_move(Square sq) {
    dec_y(sq);
    Bitmap map = gen_pos(sq);

    dec_y(sq);
    map |= gen_pos(sq);

    return map;
}


Bitmap gen_pattern(Square q, Piece p) {
    if (p == W_KING) {
        return gen_kings(q);
    }
    switch (p) {
        case B_QUEEN:
        case W_QUEEN: return gen_diags(q) | gen_ortho(q);
        case B_BISHOP:
        case W_BISHOP: return gen_diags(q);
        case B_ROOK:
        case W_ROOK: return gen_ortho(q);
        case B_KNIGHT:
        case W_KNIGHT: return gen_knights(q);
        case B_KING:
        case W_KING: return gen_kings(q);

        case W_PAWN: return gen_wpawn_move(q);
        case B_PAWN: return gen_bpawn_move(q);
        default: return (Bitmap) 0;
    }
}


Bitmap gen_vacancy_map(const Board & b) {
    Bitmap acc = (Bitmap) 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square pos = mksq(i, j);
            acc = b.get(pos) == EMPTY ? acc | gen_pos(pos) : acc;
        }
    }
    return acc;
}


#endif
