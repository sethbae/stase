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

/**
 * Other helpers: minor/major pieces
 */
constexpr bool is_minor_piece(Piece p) {
    switch (type(p)) {
        case KNIGHT:
        case BISHOP:
            return true;
        default:
            return false;
    }
}
constexpr bool is_major_piece(Piece p) {
    switch (type(p)) {
        case QUEEN:
        case ROOK:
            return true;
        default:
            return false;
    }
}
constexpr bool is_not_pk(Piece p) {
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

/**
 * Conversion to and from chars.
 */
constexpr char ptoc(Piece p) {
    switch (p) {
        case B_KING:    return 'k';
        case B_QUEEN:   return 'q';
        case B_ROOK:    return 'r';
        case B_BISHOP:  return 'b';
        case B_KNIGHT:  return 'n';
        case B_PAWN:    return 'p';
        case W_KING:    return 'K';
        case W_QUEEN:   return 'Q';
        case W_ROOK:    return 'R';
        case W_BISHOP:  return 'B';
        case W_KNIGHT:  return 'N';
        case W_PAWN:    return 'P';
        default:        return '.';
    }
}

constexpr Piece ctop(char c) {
    switch (c) {
        case 'K':   return W_KING;
        case 'Q':   return W_QUEEN;
        case 'R':   return W_ROOK;
        case 'B':   return W_BISHOP;
        case 'N':   return W_KNIGHT;
        case 'P':   return W_PAWN;
        case 'k':   return B_KING;
        case 'q':   return B_QUEEN;
        case 'r':   return B_ROOK;
        case 'b':   return B_BISHOP;
        case 'n':   return B_KNIGHT;
        case 'p':   return B_PAWN;
        default:    return EMPTY;
    }
}

// for algebraic notation (SAN), even black pieces use capitals: B_KNIGHT <----> "N"
constexpr std::string_view ptos_alg(Piece p) {
    switch (type(p)) {
        case KING: return "K";
        case QUEEN: return "Q";
        case ROOK: return "R";
        case BISHOP: return "B";
        case KNIGHT: return "N";
        case PAWN: return "";
        default: return "";
    }
}

#endif //STASE_BASE_TYPES_H
