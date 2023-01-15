#ifndef STASE_UTILS_H
#define STASE_UTILS_H

#include <sstream>
#include <string>

#include "../../include/stase/board.h"

// converts floating point to two decimal places
std::string dp_2(double);

template <typename T>
constexpr T max(const T a, const T b) {
    if (a > b) { return a; }
    return b;
}
template <typename T>
constexpr T max(const T a, const T b, const T c) {
    if (a > b) {
        return a > c ? a : c;
    }
    return b > c ? b : c;
}

template <typename T>
constexpr T min(const T a, const T b) {
    if (a < b) { return a; }
    return b;
}
template <typename T>
constexpr T min(const T a, const T b, const T c) {
    if (a < b) {
        return a < c ? a : c;
    }
    return b < c ? b : c;
}

template <typename T>
constexpr int signum(const T a, const T b) {
    if (a > b) {
        return 1;
    } else if (a < b) {
        return -1;
    }
    return 0;
}


Move uci2move(const std::string &);
std::string move2uci(const Move);

void print_stack_trace_and_abort(int sig);

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

/**
 * Splits a string around a given delimiter, by default space.
 */
inline void split(std::vector<std::string> & output, const std::string & s, const char delim = ' ') {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        output.push_back(item);
    }
}

#endif //STASE_UTILS_H
