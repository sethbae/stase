#ifndef BITMAP_H
#define BITMAP_H

#include "board.h"

/**
 * This file defines the Bitmap implementation. A Bitmap map is essentially a 64-bit number
 * that records information for any specific boolean operation you would want to perform on
 * every square. For example, a vacancy bitmap would encode every vacant square with a 1
 * and every occupied square with a 0. To get the occupation map you can simply inverse all 
 * the bits. 
 * 
 * The least significant bit in a bitmap represents a1 while the most significant represents
 * h8. The bits in a bit map are laid out on the board like this:
 * 
 *      8  |  56  57  58  59  60  61  62  63
 *         |
 *      7  |  48  49  50  51  52  53  54  55
 *         |
 *      6  |  40  41  42  43  44  45  46  47
 *         |
 *      5  |  32  33  34  35  36  37  38  39
 *         |
 *      4  |  24  25  26  27  28  29  30  31
 *         |
 *      3  |  16  17  18  19  20  21  22  23
 *         |
 *      2  |  08  09  10  11  12  13  14  15
 *         |
 *      1  |  00  01  02  03  04  05  06  07
 *          ---------------------------------
 *            a   b   c   d   e   f   g   h
 * 
 * You can see for example that c3 is represented by bit 18, d6 is 43, h2 is 15, etc.
 * 
 * Bit operations on the map simply shift the numbers left, right, up or down. There also
 * some hard coded lines and diagonals that are used to generate masks that can then be applied
 * on each other to create data.
 * 
 * 
 */

typedef uint64_t Bitmap;

// Calculated by hand
static const Bitmap col_1 = 72340172838076673L;      // a file
static const Bitmap row_1 = 255L;                  // first rank

static const Bitmap diag_nxy = 72624976668147840L;    // a8 to h1
static const Bitmap diag_xy = -9205322385119247871L;  // a1 to h8

// Generates a bitmap with only one square
Bitmap gen_pos(Square sq) {
    return ((Bitmap) 1) << get_x(sq) << (get_y(sq) * 8);
}

// These four functions generate a mask in any of the four
// orientations of a straight line on the board. Namely, row, 
// column, positive diagonal, and negative diagonal
Bitmap gen_col(Byte col) { return col_1 << col; }

Bitmap gen_row(Byte row) { return row_1 << (row * 8); }

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


// These generate multiple rows and columns. Useful for masking out big chunks of the board at a time.
Bitmap gen_mcol(int from, int to) {
    return (row_1 >> (7 - (to - from)) << from) * col_1;
}

Bitmap gen_mrow(int from, int to) {
    return (~((Bitmap) 0)) >> ((7 - (to - from)) * 8) << (from * 8);
}

// The following functions generate the basic move patterns of 
// chess pieces from the functions defined above
Bitmap gen_diags(Square sq) {
    return gen_nxy(sq) | gen_xy(sq);
}

Bitmap gen_ortho(Square sq) {
    return gen_col(get_x(sq)) | gen_row(get_y(sq));
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


// Generates the piece's movement pattern starting from a certain square
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

// Generates the vacancy map of a board
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

// Generates a bitmap according to a provided boolean function
Bitmap gen_bitmap(const Board & b, bool include(Square, Piece)) {
    Bitmap map = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square sq = mksq(i, j);
            if (include(sq, b.get(sq))) {
                map |= gen_pos(sq);
            }
        }
    }
    return map;
}


#endif
