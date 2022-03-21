#ifndef STASE_UTILS_H
#define STASE_UTILS_H

#include <sstream>
#include <string>

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

void print_stack_trace(int sig);

/**
 * Returns a string representation of the memory address pointed to by the given pointer.
 */
template<typename T>
inline std::string to_string(const T * ptr) {
    const void * p = static_cast<const void*>(ptr);
    std::stringstream ss;
    ss << p;
    return ss.str();
}

#endif //STASE_UTILS_H
