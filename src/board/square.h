#ifndef STASE_SQUARE_H
#define STASE_SQUARE_H

#include "board.h"

/**
 * The Square struct is a coordinate pair. It's easier than passing two parameters
 * or using two variables.
 */
struct Square {
    Byte x;
    Byte y;
};

const Square SQUARE_SENTINEL{0xFF, 0xFF};

/**
 * Some very basic utility functions.
 */
constexpr Square mksq(Byte x, Byte y) { return Square{x, y}; }
inline bool equal(const Square & a, const Square & b) { return a.x == b.x && a.y == b.y; }
inline bool is_sentinel(const Square & s) { return s.x == 0xFF || s.y == 0xFF; }

inline int get_y(const Square & s) { return s.y; }
inline int get_x(const Square & s) { return s.x; }

inline bool val(const Square & s) {
    return s.x < 8 && s.y < 8;
}

/**
 * String conversion functions.
 */
inline std::string sqtos(const Square sq) {
    return std::string{(char)('a' + ((char)sq.x))} + std::string{(char)('1' + ((char)sq.y))};
}
constexpr Square stosq(std::string_view str) {
    return mksq(str[0] - 'a', str[1] - '1');
}

#endif //STASE_SQUARE_H
