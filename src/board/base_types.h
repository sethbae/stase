#ifndef STASE_BASE_TYPES_H
#define STASE_BASE_TYPES_H

typedef uint_fast8_t Byte;
typedef int_fast8_t SignedByte;
typedef uint_fast32_t Int;

/**
 * The piece enum represents pieces of wood (fully specified pieces, 'white pawn').
 * This enum also includes the value EMPTY, which represents an empty square.
 */
enum Piece : Byte {

    B_KING = 0,
    B_QUEEN = 1,
    B_ROOK = 2,
    B_KNIGHT = 3,
    B_BISHOP = 4,
    B_PAWN = 5,

    // it is important that the white values are equal to 8 + the black value of the
    // same type. we rely on this later for the bit hacking.
    W_KING = 8,
    W_QUEEN = 9,
    W_ROOK = 10,
    W_KNIGHT = 11,
    W_BISHOP = 12,
    W_PAWN = 13,

    // as explained below, the EMPTY value needs to have the 5th bit (16) set.
    EMPTY = 24,

    INVALID_PIECE = 25
};

/**
 * The Ptype enum represents abstract types which pieces may have, regardless of
 * their colour. These take the same values as the Piece enum, but without the
 * fourth bit (8) set.
 */
enum Ptype : Byte {

    KING = 0,
    QUEEN = 1,
    ROOK = 2,
    KNIGHT = 3,
    BISHOP = 4,
    PAWN = 5,

    INVALID_TYPE = 16

};

/**
 * The Colour enum represents the two colours White and Black. They correspond to values
 * with and without the fourth bit set.
 */
enum Colour : Byte {
    BLACK = 0,
    WHITE = 8,
    INVALID_COLOUR = 16,
};

/**
 * In order to convert between a Piece and its type or colour, we need to check the
 * fourth bit (from the right; 8). We would also like to ensure that:
 *      type(EMPTY) != type(p) for any piece p
 *      colour(EMPTY) != type(p) for any piece p
 * We can achieve this using the 5th bit, 16, which is set in empty (and the INVALIDS).
 * Our masks therefore single out these two bits.
 *
 * To get the type, we wish to clear the 3rd bit (8), keeping the lower bits and the EMPTY bit.
 *
 * To get the colour, we wish only to retain the 3rd bit (8) and the EMPTY bit.
 */
const Byte TYPE_MASK = 0b00010111;
const Byte COLOUR_MASK = 0b00011000;

constexpr Ptype type(const Piece p) {
    return (Ptype) (p & TYPE_MASK);
}

constexpr Colour colour(const Piece p) {
    return (Colour) (p & COLOUR_MASK);
}

#endif //STASE_BASE_TYPES_H
