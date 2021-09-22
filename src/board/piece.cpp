#include "board.h"

/* lookup tables for querying information about a given piece. The 4-bit piece (0-14 valid)
   is used as an index into the relevant table */

const Ptype PIECE[] = {
        B_KING, B_QUEEN, B_ROOK, B_KNIGHT, B_BISHOP, B_PAWN,        // indexes 0-5
        INVALID, INVALID,                                           // indexes 6, 7
        W_KING, W_QUEEN, W_ROOK, W_KNIGHT, W_BISHOP, W_PAWN,        // indexes 8-13
        EMPTY                                                       // index 14
};

const Ptype TYPE[] = {
        KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN,    // indexes 0-5
        INVALID, INVALID,                           // indexes 6, 7
        KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN,    // indexes 8-13
        EMPTY                                       // index 14
};

const Ptype COLOUR[] = {
        BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,   // indexes 0-5
        INVALID, INVALID,                           // indexes 6, 7
        WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,   // indexes 8-13
        EMPTY                                       // index 14
};

Ptype piece(Piece p) { return PIECE[p]; }
Ptype type(Piece p) { return TYPE[p]; }
Ptype colour(Piece p) { return COLOUR[p]; }
bool is_white(Piece p) { return p & 8; }
bool is_minor_piece(Piece p) {
    switch (type(p)) {
        case KNIGHT:
        case BISHOP:
            return true;
        default:
            return false;
    }
}
bool is_major_piece(Piece p) {
    switch (type(p)) {
        case QUEEN:
        case ROOK:
            return true;
        default:
            return false;
    }
}
bool is_not_pk(Piece p) {
    switch (type(p)) {
        case QUEEN:
        case ROOK:
        case KNIGHT:
        case BISHOP:
            return true;
        default:
            return false;
    }
}
