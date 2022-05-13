#ifndef STASE_EVAL_HPP
#define STASE_EVAL_HPP

using std::stringstream;
#include <string>
using std::string;
#include <iomanip>
#include <cmath>

typedef int32_t Eval;

const Eval BLACK_GIVES_MATE = 0x80000000;
const Eval WHITE_GIVES_MATE = 0x7FFFFFFF;

// two masks for checking for mate
const Eval BLACK_MATE_MASK = 0x80000000;
const Eval WHITE_MATE_MASK = 0x40000000;

// 14 bits are used to represent actual evaluations; these are the positive numbers 0-16384
// We then use an offset when needed to map these to -8192 <= x <= 8191
//      N.B. for correct behaviour, the offset must be half the total range
const Eval OFFSET = 0x20000000;

constexpr Eval zero() {
    return OFFSET;
}

constexpr Eval white_mates_in(unsigned num) {
    return WHITE_GIVES_MATE - num;
}

constexpr Eval black_mates_in(unsigned num) {
    return BLACK_GIVES_MATE + num;
}

/**
 * Returns an evaluation which represents checkmate in one move more than that already
 * represented. Eg, #1 becomes #2, #-4 becomes #-5. If the given eval does not represent
 * checkmate, then zero is returned.
 */
constexpr Eval mate_in_one_more(const Eval e) {
    if (e & WHITE_MATE_MASK) {
        return e - 1;
    } else if (e & BLACK_MATE_MASK) {
        return e + 1;
    } else {
        return (Eval) 0;
    }
}

constexpr Eval white_has_been_mated() {
    return BLACK_GIVES_MATE;
}

constexpr Eval black_has_been_mated() {
    return WHITE_GIVES_MATE;
}

constexpr bool is_mate(const Eval eval) {
    return (eval & BLACK_MATE_MASK) | (eval & WHITE_MATE_MASK);
}

constexpr bool white_is_mated(const Eval eval) {
    return eval & BLACK_MATE_MASK;
}

constexpr bool black_is_mated(const Eval eval) {
    return eval & WHITE_MATE_MASK;
}

// convert the Eval to a float representing a more traditional human evaluation
inline float human_eval(const int eval) {
    if (is_mate(eval)) {
        // cannot be represented as a float
        return nanf("");
    }
    return ((float) eval - OFFSET) / 1000.0f;
}

inline unsigned millipawn_diff(const Eval a, const Eval b) {
    return std::abs((long int)a - (long int)b);
}

/**
 * Converts an evaluation to a human readable string, accounting for checkmates.
 */
inline std::string etos(const Eval e) {

    // handle checkmates
    if (e == white_has_been_mated()) {
        return "-#";
    } else if (e == black_has_been_mated()) {
        return "#";
    } else if (e & WHITE_MATE_MASK) {
        return "#" + std::to_string(WHITE_GIVES_MATE - e);
    } else if (e & BLACK_MATE_MASK) {
        return "#" + std::to_string(BLACK_GIVES_MATE - e);
    }

    // handle numerical evaluations
    stringstream ss;
    ss << std::fixed << std::setprecision(3) << human_eval(e);
    if (e < zero()) {
        return "+" + ss.str();
    } else {
        return ss.str();
    }
}

#endif
