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

/* helper functions for getting info about a square */
inline int get_y(const Square & s) { return s.y; }
inline int get_x(const Square & s) { return s.x; }

inline bool val(const Square & s) {
    return s.x < 8 && s.y < 8;
}

#endif //STASE_SQUARE_H
