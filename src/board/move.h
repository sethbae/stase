#ifndef STASE_MOVE_H
#define STASE_MOVE_H

#include "board.h"

/**
 * The Move struct stores a starting square and an ending square.
 *
 * There are also some flags, which represent information about that move,
 * along with appropriate accessors and mutators (since the flags are bit
 * hacks, this saves a lot of trouble).
 */
struct Move {

    Square from;
    Square to;
    uint_fast16_t flags;

    bool is_prom() const;
    void set_prom();
    void unset_prom();

    bool is_cas() const;
    void set_cas();
    void unset_cas();

    bool is_ep() const;
    void set_ep();
    void unset_ep();

    bool is_cap() const;
    void set_cap();
    void unset_cap();

    bool is_cas_short() const;
    void set_cas_short();
    void unset_cas_short();

    int get_ep_file() const;

    Byte get_prom_shift() const;
    Piece get_prom_piece(Colour) const;
    void set_prom_piece(Ptype);
    Piece get_cap_piece() const;
    void set_cap_piece(Piece p);

    int get_score() const;
    void set_score(int);
    void inc_score(int);

};

/**
 * Some basic special values and utility functions.
 */
const Move MOVE_SENTINEL = Move{SQUARE_SENTINEL, SQUARE_SENTINEL, 0};
constexpr Move empty_move() {
    return Move{0, 0, 0, 0, 0};
};
inline bool is_sentinel(const Move m) { return equal(m.from, SQUARE_SENTINEL); }
inline bool equal(const Move m1, const Move m2) {
    return equal(m1.from, m2.from) && equal(m1.to, m2.to);
}

/**
 * The MoveType enum represents the different manners in which a piece can move
 * on the board.
 */
enum MoveType {
    ORTHO = 0,
    DIAG = 1,
    KNIGHT_MOVE = 2,
    PAWN_MOVE = 3,
    KING_MOVE = 4,
    INVALID_MOVE = 5
};

/**
 * Deltas are useful for lightweight passing of direction parameters.
 */
struct Delta {
    SignedByte dx;
    SignedByte dy;
};
const Delta INVALID_DELTA = { 127, 127 };
const Delta KNIGHT_DELTA = { 1, 2 };
inline bool is_valid_delta(const Delta d) { return d.dx != 127 && d.dy != 127; }
inline bool equal(const Delta d, const Delta d2) { return d.dx == d2.dx && d.dy == d2.dy; }
inline Delta delta(int x, int y) { return Delta{(SignedByte) x, (SignedByte) y}; }
inline MoveType direction_of_delta(const Delta d) {
    return (d.dx == 0 || d.dy == 0) ? ORTHO : DIAG;
}
inline Delta get_delta_between(const Square a, const Square b){
    SignedByte dx = b.x - a.x;
    SignedByte dy = b.y - a.y;
    if (dx != 0) {
        dx = (dx > 0) ? 1 : -1;
    }
    if (dy != 0) {
        dy = (dy > 0) ? 1 : -1;
    }
    return Delta{dx, dy};
};

/**
 * Arrays for diffs which can be added to a square; e.g. (1,1) to move diagonally up right.
 */
extern const SignedByte XD[];
extern const SignedByte YD[];
extern const SignedByte XKN[];
extern const SignedByte YKN[];

// constants for iterating over the directions
const unsigned DIAG_START  = 0;
const unsigned DIAG_STOP   = 4;
const unsigned ORTHO_START = 4;
const unsigned ORTHO_STOP  = 8;
const unsigned ALL_DIRS_START = 0;
const unsigned ALL_DIRS_STOP = 8;

/**
 * String conversions.
 */
class Board;
Move stom(const Board &, const std::string &);
std::string mtos(const Board &, const Move);

#endif //STASE_MOVE_H
