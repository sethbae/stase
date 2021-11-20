#ifndef STASE_BASE_TYPES_H
#define STASE_BASE_TYPES_H

typedef uint_fast8_t Byte;
typedef uint_fast32_t Int;
typedef Byte Piece;
typedef uint64_t Bitmap;

/* enumeration of the types of pieces primarily, but also for other useful properties,
    such as WHITE or BLACK, and KING or QUEEN */
enum Ptype : Byte {

    /* defined values: so that we can translate both ways */
    B_KING = 0,
    B_QUEEN = 1,
    B_ROOK = 2,
    B_KNIGHT = 3,
    B_BISHOP = 4,
    B_PAWN = 5,

    W_KING = 8,
    W_QUEEN = 9,
    W_ROOK = 10,
    W_KNIGHT = 11,
    W_BISHOP = 12,
    W_PAWN = 13,

    EMPTY = 14,

    /* other values which appear in the lookup tables */
    KING,
    QUEEN = 16, // explicit values to read promotion pieces easily (see Move::prom_piece())
    ROOK = 17,
    KNIGHT = 18,
    BISHOP = 19,
    PAWN,

    BLACK,
    WHITE,

    INVALID
};

#endif //STASE_BASE_TYPES_H
