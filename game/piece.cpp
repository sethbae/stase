#include "game.h"
#include "../board/board.h"

#include <iostream>
using std::cout;

/*

   This file implements various notions of control which may be useful in multiple
   areas of the program. There are three types of control:
 
        Alpha control:      The empty squares a piece can move to
        Beta control:       Alpha squares, plus those squares which contain a piece
        Gamma control:      Beta squares, plus those squares accessible by x-ray
        
   Thus far, these control definitions apply to QUEENS, ROOKS, BISHOPS and KNIGHTS.
   
   For each type, there is a 'control' method which counts the number of such squares
   as well as a 'covers' method which checks whether the piece controls the square.
   
   The 'covers' method is defined for all piece types.
 
 */

enum MoveType {
    ORTHO = 0,
    DIAG = 1,
    KNIGHT_MOVE = 2,
    PAWN_MOVE = 3,
    KING_MOVE = 4,
    INVALID_MOVE = 5
};

enum Direction {
    ORTHO_R = 0,
    ORTHO_L = 1,
    ORTHO_U = 2,
    ORTHO_D = 3,
    DIAG_UR = 4,
    DIAG_UL = 5,
    DIAG_DR = 6,
    DIAG_DL = 7
};

// constants for iterating over the directions
const unsigned ORTHO_START = 0;
const unsigned ORTHO_STOP  = 4;
const unsigned DIAG_START  = 4;
const unsigned DIAG_STOP   = 8;

// lookup tables for the step function which moves in each direction
typedef void StepFunc(Square &);
const StepFunc *STEP_FUNCS[] = {
    &inc_x,
    &dec_x,
    &inc_y,
    &dec_y,
    &diag_ur,
    &diag_ul,
    &diag_dr,
    &diag_dl
};

// and a lookup table (indexed by numeric value of Ptype enumeration) for which of
// the above movesets to use.
const bool PIECE_MOVE_TYPES[][6] = {
    { false, false, false, false, true, false },    // KING
    { true, true, false, false, false, false },     // QUEEN
    { true, false, false, false, false, false },    // ROOK
    { false, false, true, false, false, false },    // KNIGHT
    { false, true, false, false, false, false },    // BISHOP
    { false, false, false, true, false, false },    // PAWN
    { false, false, false, false, false, false },   // ---------invalid-----------
    { false, false, false, false, false, false },   // ---------invalid-----------
    { false, false, false, false, true, false },    // KING
    { true, true, false, false, false, false },     // QUEEN
    { true, false, false, false, false, false },    // ROOK
    { false, false, true, false, false, false },    // KNIGHT
    { false, true, false, false, false, false },    // BISHOP
    { false, false, false, true, false, false },    // PAWN
    { false, false, false, false, false, false }    // ---------invalid-----------
};

// returns true if the given piece can move in the given way, false otherwise
bool can_move(Piece piece, MoveType dir) {
    return PIECE_MOVE_TYPES[piece][dir];
}

// performs a linear search on the board according to the given step and val functions.
// returns the alpha count of the walk
inline unsigned alpha_walk(const Board & b, const Square s, StepFunc *step) {
    
    unsigned sum = 0;
    Square temp = s;
    bool cont = true;
    
    (*step)(temp);
    
    while (val(temp) && cont) {

        Piece otherp = b.get(temp);
        if (type(otherp) == EMPTY) {
            ++sum;
        } else {
            // ALPHA: only count empty squares
            cont = false;
        }
        (*step)(temp);
    }
    
    return sum;
}

// computes and returns the number of squares which the given piece 'alpha controls'
unsigned alpha_control(const Board & b, const Square s) {
    
    unsigned sum = 0;
    
    Piece p = b.get(s);
    
    if (can_move(p, ORTHO)) {
        for (unsigned dir = ORTHO_START; dir < ORTHO_STOP; ++dir) {
            sum += alpha_walk(b, s, STEP_FUNCS[dir]);
        }
    }
    
    if (can_move(p, DIAG)) {
        for (unsigned dir = DIAG_START; dir < DIAG_STOP; ++dir) {
            sum += alpha_walk(b, s, STEP_FUNCS[dir]);
        }
    }
    
    if (can_move(p, KNIGHT_MOVE)) {
    
        unsigned x = get_x(s);
        unsigned y = get_y(s);
        Square sq;
        
        if (val(sq = mksq(x + 1, y + 2)) && b.get(sq) == EMPTY)
            ++sum;
        if (val(sq = mksq(x + 1, y - 2)) && b.get(sq) == EMPTY)
            ++sum;
        if (val(sq = mksq(x + 2, y + 1)) && b.get(sq) == EMPTY)
            ++sum;
        if (val(sq = mksq(x + 2, y - 1)) && b.get(sq) == EMPTY)
            ++sum;
        if (val(sq = mksq(x - 1, y + 2)) && b.get(sq) == EMPTY)
            ++sum;
        if (val(sq = mksq(x - 1, y - 2)) && b.get(sq) == EMPTY)
            ++sum;
        if (val(sq = mksq(x - 2, y + 1)) && b.get(sq) == EMPTY)
            ++sum;
        if (val(sq = mksq(x - 2, y - 1)) && b.get(sq) == EMPTY)
            ++sum;
    }
    
    return sum;

}

// performs a linear search on the board according to the given step and val functions.
// returns the beta count of the walk
inline unsigned beta_walk(const Board & b, const Square s, StepFunc *step) {
    
    unsigned sum = 0;
    Square temp = s;
    bool cont = true;
    
    (*step)(temp);
    
    while (val(temp) && cont) {

        Piece otherp = b.get(temp);
        if (type(otherp) == EMPTY) {
            ++sum;
        } else {
            // BETA: count the endpoint
            cont = false;
            ++sum;
        }
        (*step)(temp);
    }
    
    return sum;
}

// computes and returns the number of squares which the given piece 'beta controls'
unsigned beta_control(const Board & b, const Square s) {
    
    unsigned sum = 0;
    
    Piece p = b.get(s);
    
    if (can_move(p, ORTHO)) {
        for (unsigned dir = ORTHO_START; dir < ORTHO_STOP; ++dir) {
            sum += beta_walk(b, s, STEP_FUNCS[dir]);
        }
    }
    
    if (can_move(p, DIAG)) {
        for (unsigned dir = DIAG_START; dir < DIAG_STOP; ++dir) {
            sum += beta_walk(b, s, STEP_FUNCS[dir]);
        }
    }
    
    if (can_move(p, KNIGHT_MOVE)) {
    
        unsigned x = get_x(s);
        unsigned y = get_y(s);
        Square sq;
        
        if (val(sq = mksq(x + 1, y + 2)))
            ++sum;
        if (val(sq = mksq(x + 1, y - 2)))
            ++sum;
        if (val(sq = mksq(x + 2, y + 1)))
            ++sum;
        if (val(sq = mksq(x + 2, y - 1)))
            ++sum;
        if (val(sq = mksq(x - 1, y + 2)))
            ++sum;
        if (val(sq = mksq(x - 1, y - 2)))
            ++sum;
        if (val(sq = mksq(x - 2, y + 1)))
            ++sum;
        if (val(sq = mksq(x - 2, y - 1)))
            ++sum;
    }
    
    return sum;

}

// performs a linear search on the board according to the given step and val functions.
// returns the gamma count of the walk
inline unsigned gamma_walk(const Board & b, const Square s, StepFunc *step, MoveType dir) {
    
    unsigned sum = 0;
    Square temp = s;
    bool cont = true;
    
    (*step)(temp);
    
    while (val(temp) && cont) {

        Piece otherp = b.get(temp);
        if (type(otherp) == EMPTY) {
            ++sum;
        } else {
            // gamma: x-ray
            cont = (colour(otherp) == colour(b.get(s))) && can_move(otherp, dir);
            ++sum;
        }
        (*step)(temp);
    }
    
    return sum;
}

// computes and returns the number of squares which the given piece 'gamma controls'
unsigned gamma_control(const Board & b, const Square s) {
    
    unsigned sum = 0;
    
    Piece p = b.get(s);
    
    if (can_move(p, ORTHO)) {
        for (unsigned dir = ORTHO_START; dir < ORTHO_STOP; ++dir) {
            sum += gamma_walk(b, s, STEP_FUNCS[dir], ORTHO);
        }
    }
    
    if (can_move(p, DIAG)) {
        for (unsigned dir = DIAG_START; dir < DIAG_STOP; ++dir) {
            sum += gamma_walk(b, s, STEP_FUNCS[dir], DIAG);
        }
    }
    
    if (can_move(p, KNIGHT_MOVE)) {
    
        unsigned x = get_x(s);
        unsigned y = get_y(s);
        Square sq;
        
        if (val(sq = mksq(x + 1, y + 2)))
            ++sum;
        if (val(sq = mksq(x + 1, y - 2)))
            ++sum;
        if (val(sq = mksq(x + 2, y + 1)))
            ++sum;
        if (val(sq = mksq(x + 2, y - 1)))
            ++sum;
        if (val(sq = mksq(x - 1, y + 2)))
            ++sum;
        if (val(sq = mksq(x - 1, y - 2)))
            ++sum;
        if (val(sq = mksq(x - 2, y + 1)))
            ++sum;
        if (val(sq = mksq(x - 2, y - 1)))
            ++sum;
    }
    
    return sum;

}

