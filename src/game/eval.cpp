#include "game.h"
#include <iostream>
using std::cout;
#include <sstream>
using std::stringstream;
#include <string>
using std::string;
#include <iomanip>

const Eval BLACK_GIVES_MATE = -32768; // 0x8000 - 16 bits with MSB set
const Eval WHITE_GIVES_MATE = 0x7FFF; // 16 bits with all but MSB set

// two masks for checking for mate
const Eval BLACK_MATE_MASK = -32768; // 0x8000;
const Eval WHITE_MATE_MASK = 0x4000;

const float MIN_EVAL = -80.0f;
const float MAX_EVAL = 80.0f;

// 14 bits are used to represent actual evaluations; these are the positive numbers 0-16384
// We then use an offset when needed to map these to -8192 <= x <= 8191
//      N.B. for correct behaviour, the offset must be half the total range
const Eval OFFSET = 0x1000; // 2^13

Eval zero() {
    return OFFSET;
}

Eval mate_in(Ptype colour, unsigned num) {
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

    /*
    if (white_is_mated(eval))
        return MIN_EVAL - 1;
    if (black_is_mated(eval))
        return MAX_EVAL + 1;
        
    // how far through the range of ratings is the current rating? 0 <= x <= 1
    float proportional_rating = eval / (2.0 * OFFSET);

    // map this onto the desired range of possible evaluations
    return MIN_EVAL + (proportional_rating * (MAX_EVAL - MIN_EVAL));
    */
    
    return ((float) eval - OFFSET) / 1000.0f;
}

int int_eval(const Eval eval) {
    return eval - OFFSET;
}

Eval eval_from_float(float f) {
    
    // if outside of range, return the corresponding checkmate
    if (f < MIN_EVAL)
        return BLACK_GIVES_MATE;
    if (f > MAX_EVAL)
        return WHITE_GIVES_MATE;
    
    // otherwise figure out how far it is through the range
    float proportional_rating = (f - MIN_EVAL) / (MAX_EVAL - MIN_EVAL);

    // and map to the actual rating range (as positive integer because of the offset scheme)
    return (unsigned) (proportional_rating * OFFSET * 2);
    
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
