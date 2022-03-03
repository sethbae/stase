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
inline bool val(const Byte x, const Byte y) {
    return x < 8 && y < 8;
}

inline bool light_square(const Square s) {
    const static bool lookup[8][8]{
        {true, false, true, false, true, false, true, false},
        {false, true, false, true, false, true, false, true},
        {true, false, true, false, true, false, true, false},
        {false, true, false, true, false, true, false, true},
        {true, false, true, false, true, false, true, false},
        {false, true, false, true, false, true, false, true},
        {true, false, true, false, true, false, true, false},
        {false, true, false, true, false, true, false, true},
    };
    return lookup[7 - s.y][s.x];
}

inline uint8_t diag_ordinal(const Square s) {
    const static uint8_t lookup[8][8]{
        {3, 4, 2, 5, 1, 6, 0, 7},
        {3, 3, 4, 2, 5, 1, 6, 0},
        {4, 3, 3, 4, 2, 5, 1, 6},
        {2, 4, 3, 3, 4, 2, 5, 1},
        {5, 2, 4, 3, 3, 4, 2, 5},
        {1, 5, 2, 4, 3, 3, 4, 2},
        {6, 1, 5, 2, 4, 3, 3, 4},
        {0, 6, 1, 5, 2, 4, 3, 3},
    };
    return lookup[s.y][s.x];
}

constexpr int sq_sentinel_as_int() { return 0xFFFF; }
constexpr int sqtoi(const Square s) {
    return (((int) s.x) << 4) | ((int) s.y);
}
constexpr Square itosq(const int i) {
    return Square{(Byte) (i >> 4), (Byte) (i & 0xF)};
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
