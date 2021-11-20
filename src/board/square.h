#ifndef STASE_SQUARE_H
#define STASE_SQUARE_H

#include "board.h"

struct Square {
    Byte x;
    Byte y;
};

const Square SQUARE_SENTINEL{0xFF, 0xFF};

constexpr Square mksq(Byte x, Byte y) { return Square{x, y}; }
inline bool equal(const Square & a, const Square & b) { return a.x == b.x && a.y == b.y; }
inline bool is_sentinel(const Square & s) { return s.x == 0xFF || s.y == 0xFF; }

/* helper functions for transforming a square */
void inc_x(Square &);
void dec_x(Square &);
void inc_y(Square &);
void dec_y(Square &);
void diag_ur(Square &);
void diag_ul(Square &);
void diag_dr(Square &);
void diag_dl(Square &);
void reset_x(Square &);
void reset_y(Square &);

/* helper functions for getting info about a square */
inline int get_y(const Square & s) { return s.y; }
inline int get_x(const Square & s) { return s.x; }

inline bool val_x(const Square & s) {
    return s.x < 8;
}
inline bool val_y(const Square & s) {
    return s.y < 8;
}
inline bool val(const Square & s) {
    return s.x < 8 && s.y < 8;
}

#endif //STASE_SQUARE_H
