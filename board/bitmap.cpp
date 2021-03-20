#include "board.h"
#include <iostream>
using std::cout;

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

// Calculated by hand
static const Bitmap col_1 = 72340172838076673L;      // a file
static const Bitmap row_1 = 255L;                  // first rank

static const Bitmap diag_nxy = 72624976668147840L;    // a8 to h1
static const Bitmap diag_xy = -9205322385119247871L;  // a1 to h8

void set_square(Bitmap & bmap, const Square sq) {
    uint64_t mask = ( (uint64_t) 1) << get_x(sq) << (get_y(sq)*8);
    bmap |= mask;
}

void unset_square(Bitmap & bmap, const Square sq) {
    uint64_t mask = ( (uint64_t) 1) << get_x(sq) << (get_y(sq)*8);
    bmap &= ~mask;
}

bool test_square(const Bitmap & bmap, const Square sq) {
    uint64_t mask = ( (uint64_t) 1) << get_x(sq) << (get_y(sq)*8);
    return bmap & mask;
}

// Generates a bitmap with only one square
Bitmap square_map(Square sq) {
    return ((Bitmap) 1) << get_x(sq) << (get_y(sq) * 8);
}

// Generates a square from a bitmap with only one square
// If a map has more than one square set, the top-most then left-most square will be returned
Square map_to_sq(Bitmap b) {
    int y = 0;
    for (; b > 255; b >>= 8) {
        ++y;
    }

    int x = 0;
    for (; b > 1; b >>= 1) {
        ++x;
    }

    return mksq(x, y);
}

// These four functions generate a mask in any of the four
// orientations of a straight line on the board. Namely, row, 
// column, positive diagonal, and negative diagonal
Bitmap column_map(unsigned col) { return col_1 << col; }

Bitmap row_map(unsigned row) { return row_1 << (row * 8); }

Bitmap negdiag_map(Square sq) {
    Bitmap new_nxy = diag_nxy;
    int dif_nxy = get_y(sq) + get_x(sq) - 7;
    if (dif_nxy > 0) {
        new_nxy = diag_nxy << (dif_nxy * 8);
    } else if (dif_nxy < 0) {
        new_nxy = diag_nxy >> (-dif_nxy * 8);
    }

    return new_nxy;
}

Bitmap posdiag_map(Square sq) {
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
Bitmap columns_map(unsigned from, unsigned to) {
    return (row_1 >> (7 - (to - from)) << from) * col_1;
}

Bitmap rows_map(unsigned from, unsigned to) {
    return (~((Bitmap) 0)) >> ((7 - (to - from)) * 8) << (from * 8);
}

// The following functions generate the basic move patterns of 
// chess pieces from the functions defined above
Bitmap diag_map(Square sq) {
    return negdiag_map(sq) | posdiag_map(sq);
}

Bitmap ortho_map(Square sq) {
    return column_map(get_x(sq)) | row_map(get_y(sq));
}

Bitmap knight_map(Square sq) {
    Bitmap queenmove = ortho_map(sq) | diag_map(sq);

    int min_x = get_x(sq) < 2 ? 0 : get_x(sq) - 2;
    int min_y = get_y(sq) < 2 ? 0 : get_y(sq) - 2;
    int max_x = get_x(sq) > 5 ? 7 : get_x(sq) + 2;
    int max_y = get_y(sq) > 5 ? 7 : get_y(sq) + 2;

    return ~queenmove & columns_map(min_x, max_x) & rows_map(min_y, max_y);
}

Bitmap king_map(Square sq) {
    int min_x = get_x(sq) < 1 ? 0 : get_x(sq) - 1;
    int min_y = get_y(sq) < 1 ? 0 : get_y(sq) - 1;
    int max_x = get_x(sq) > 6 ? 7 : get_x(sq) + 1;
    int max_y = get_y(sq) > 6 ? 7 : get_y(sq) + 1;

    return columns_map(min_x, max_x) & rows_map(min_y, max_y);
}

Bitmap pawn_attack_map(Square sq, Ptype colour) {
    Bitmap map = 0;

    int step = colour == WHITE ? 1 : -1;
    int y = get_y(sq) + step;
    int x = get_x(sq);

    Square temp = mksq(x + 1, y);
    if (val_x(temp)) {
        set_square(map, temp);
    }

    temp = mksq(x - 1, y);
    if (val_x(temp)) {
        set_square(map, temp);
    }

    return map;
}

Bitmap pawn_move_map(Square sq, Ptype colour) {
    int step = colour == WHITE ? 1 : -1;

    Bitmap map = 0;

    set_square(map, mksq(get_x(sq), get_y(sq) + step));

    if (get_y(sq) == 1 || get_y(sq) == 6) {
        set_square(map, mksq(get_x(sq), get_y(sq) + (step * 2)));
    }

    return map;
}

// Generates the piece's movement pattern starting from a certain square
Bitmap pattern_map(Square q, Piece p) {
    switch (type(p)) {
        case QUEEN: return diag_map(q) | ortho_map(q);
        case BISHOP: return diag_map(q);
        case ROOK: return ortho_map(q);
        case KNIGHT: return knight_map(q);
        case KING: return king_map(q);
        case PAWN: return pawn_move_map(q, colour(p));
        default: return (Bitmap) 0;
    }
}

// Generates the vacancy map of a board
Bitmap vacancy_map(const Board & b) {
    Bitmap acc = (Bitmap) 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square pos = mksq(i, j);
            if (b.get(pos) == EMPTY) {
                set_square(acc, pos);
            }
            // acc = b.get(pos) == EMPTY ? acc | square_map(pos) : acc;
        }
    }
    return acc;
}


Bitmap occupancy_map(const Board & b) {
    Bitmap acc = (Bitmap) 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square pos = mksq(i, j);
            if (b.get(pos) != EMPTY) {
                set_square(acc, pos);
            }
        }
    }
    return acc;
}


Bitmap friendly_map(const Board & b) {
    Bitmap acc = (Bitmap) 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square pos = mksq(i, j);
            if (colour(b.get(pos)) == b.colour_to_move()) {
                set_square(acc, pos);
            }
        }
    }
    return acc;
}


Bitmap enemy_map(const Board & b) {
    Bitmap acc = (Bitmap) 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square pos = mksq(i, j);
            if (colour(b.get(pos)) != b.colour_to_move()) {
                set_square(acc, pos);
            }
        }
    }
    return acc;
}


Bitmap attack_search(const Board & b, const Square s,
                    void step(Square &), 
                    bool (* valid)(const Square &), int depth) {
    
    Bitmap bmap = (Bitmap) 0;
    
    //Piece p = b.get(s);
    Square temp = s;
    int level = 1;

    step(temp);
    
    while ((*valid)(temp)) {
        //cout << sqtos(temp);
        Piece otherp = b.get(temp);
        set_square(bmap, temp);
        if (type(otherp) != EMPTY && (level += 1) > depth) {
            return bmap;
        }
        step(temp);
    }
    
    return bmap;
}

Bitmap ortho_attack(const Board & b, const Square start_sq, int depth = 1) {
    
    return attack_search(b, start_sq, inc_x, val_x, depth) 
            | attack_search(b, start_sq, dec_x, val_x, depth)
            | attack_search(b, start_sq, inc_y, val_y, depth)
            | attack_search(b, start_sq, dec_y, val_y, depth);
    
}

Bitmap diag_attack(const Board & b, const Square start_sq, int depth = 1) {
    
    return attack_search(b, start_sq, diag_ur, val, depth)
            | attack_search(b, start_sq, diag_dl, val, depth)
            | attack_search(b, start_sq, diag_dr, val, depth)
            | attack_search(b, start_sq, diag_ul, val, depth);
    
}

Bitmap attack_moves(const Board & b, const Square s) {
    
    Piece p = b.get(s);

    switch (type(p)) {
        
        case ROOK: 
            return ortho_attack(b, s);
            
        case BISHOP: 
            return diag_attack(b, s);
            
        case QUEEN: 
            return ortho_attack(b, s) | diag_attack(b, s);
            
        case KNIGHT:
            return knight_map(s);
            
        case KING:
            return king_map(s);
            
        case PAWN:
            return pawn_attack_map(s, colour(p));
            
        default: 
            return (Bitmap) 0;
    }

}


Bitmap attack_map(const Board & b, Ptype c) {
    Bitmap acc = (Bitmap) 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square pos = mksq(i, j);
            if (colour(b.get(pos)) == c) {
                acc |= attack_moves(b, pos);
            }
        }
    }
    return acc;
}


Bitmap piece_map(const Board & b, Ptype piece) {
    Bitmap acc = (Bitmap) 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square pos = mksq(i, j);
            if (type(b.get(pos)) == piece) {
                set_square(acc, pos);
            }
        }
    }
    return acc;
}


// Generates a bitmap according to a provided boolean function
Bitmap custom_map(const Board & b, bool include(const Board &, Square)) {
    Bitmap map = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square sq = mksq(i, j);
            if (include(b, sq)) {
                set_square(map, sq);
            }
        }
    }
    return map;
}

