#ifndef STASE_UTILS_H
#define STASE_UTILS_H

#include "board.h"

// converts floating point to two decimal places
std::string dp_2(double);

template <typename T>
constexpr T max(const T a, const T b) {
    if (a > b) { return a; }
    return b;
}

template <typename T>
constexpr T min(const T a, const T b) {
    if (a < b) { return a; }
    return b;
}

Move uci2move(const std::string &);
std::string move2uci(const Move);

#endif //STASE_UTILS_H
