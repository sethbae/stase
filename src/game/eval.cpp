#include "game.h"
#include <iostream>
using std::cout;
#include <sstream>
using std::stringstream;
#include <string>
using std::string;
#include <iomanip>
#include <cmath>

const Eval BLACK_GIVES_MATE = 0x80000000; // 0x8000 - 16 bits with MSB set
const Eval WHITE_GIVES_MATE = 0x7FFFFFFF; // 16 bits with all but MSB set

// two masks for checking for mate
const Eval BLACK_MATE_MASK = 0x80000000; // 0x8000;
const Eval WHITE_MATE_MASK = 0x40000000;

// 14 bits are used to represent actual evaluations; these are the positive numbers 0-16384
// We then use an offset when needed to map these to -8192 <= x <= 8191
//      N.B. for correct behaviour, the offset must be half the total range
const Eval OFFSET = 0x2000000; // 2^13

Eval mate_in(Colour colour, unsigned num) {
    return (colour == WHITE) ? white_mates_in(num) : black_mates_in(num);
}

Eval white_mates_in(unsigned num) {
    return WHITE_GIVES_MATE - num;
}

Eval black_mates_in(unsigned num) {
    return BLACK_GIVES_MATE + ((Eval)num);
}

/**
 * Returns an evaluation which represents checkmate in one move more than that already
 * represented. Eg, #1 becomes #2, #-4 becomes #-5. If the given eval does not represent
 * checkmate, then zero is returned.
 */
Eval mate_in_one_more(const Eval e) {
    if (e & WHITE_GIVES_MATE) {
        return e - 1;
    } else if (e & BLACK_GIVES_MATE) {
        return e + 1;
    } else {
        return (Eval) 0;
    }
}

Eval white_has_been_mated() {
    return BLACK_GIVES_MATE;
}

Eval black_has_been_mated() {
    return WHITE_GIVES_MATE;
}

bool is_mate(const Eval eval) {
    return (eval & BLACK_MATE_MASK) | (eval & WHITE_MATE_MASK);
}

bool white_is_mated(const Eval eval) {
    return eval & BLACK_MATE_MASK;
}

bool black_is_mated(const Eval eval) {
    return eval & WHITE_MATE_MASK;
}

// convert the Eval to a float representing a more traditional human evaluation
float human_eval(const int eval) {

    if (is_mate(eval)) {
        // cannot be represented as a float
        return nanf("");
    }
    return ((float) eval - OFFSET) / 1000.0f;
}

int int_eval(const Eval eval) {
    return eval - OFFSET;
}

/**
 * Converts an evaluation to a human readable string, accounting for checkmates.
 */
string etos(const Eval e) {

    // handle checkmates
    if (e == white_has_been_mated() || e == black_has_been_mated()) {
        return "#";
    } else if (e & WHITE_MATE_MASK) {
        return "#" + std::to_string(WHITE_GIVES_MATE - e);
    } else if (e & BLACK_MATE_MASK) {
        return "#" + std::to_string(BLACK_GIVES_MATE - e);
    }

    // handle numerical evaluations
    stringstream ss;
    ss << std::fixed << std::setprecision(3) << human_eval(e);
    return ss.str();
}
