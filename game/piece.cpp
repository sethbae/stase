#include "game.h"
#include "../board/board.h"

#include <iostream>
using std::cout;

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


/*
    walks outward from the given square. Returns an integer representing the resulting control
    change from that walk. 
    So if it first encounters a white piece that can move in the right way, it goes +1 but 
    countinues looking for a different piece along the same diagonal which may be able to x-ray 
    control the square.
    
    This method ignores pawns and kings.
*/
int control_walk(const Board & b, const Square s, StepFunc *step, MoveType dir) {

    unsigned sum = 0;
    Square temp = s;
    bool cont = true;
    
    (*step)(temp);
    
    while (val(temp) && cont) {

        Piece otherp = b.get(temp);

        if ((type(otherp) != EMPTY) && can_move(otherp, dir)) {
            // any piece which can move in the right dir? account and continue
            sum += ((colour(otherp) == WHITE) ? 1 : -1);
        } else  if (type(otherp) != EMPTY) {
            // blocking piece, abort
            cont = false;
        }
        (*step)(temp);
    }
    
    return sum;

}

/*
    Returns an integer representing the number of pieces which control the given square.
    Returns a positive int for white, negative for black
    This accounts for kings, knights and pawns, and delegates sliding pieces to the method
    above.
*/
int control_count(const Board & b, const Square s) {

    int count = 0;

    // orthogonal movement
    for (unsigned dir = ORTHO_START; dir < ORTHO_STOP; ++dir) {
        count += control_walk(b, s, STEP_FUNCS[dir], ORTHO);
    }

    // diagonal movement
    for (unsigned dir = DIAG_START; dir < DIAG_STOP; ++dir) {
        count += control_walk(b, s, STEP_FUNCS[dir], DIAG);
    }
    
    
    unsigned x = get_x(s);
    unsigned y = get_y(s);
    Square sq;
    
    // knights
    if (val(sq = mksq(x + 1, y + 2)) && (type(b.get(sq)) == KNIGHT))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x + 1, y - 2)) && (type(b.get(sq)) == KNIGHT))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x + 2, y + 1)) && (type(b.get(sq)) == KNIGHT))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x + 2, y - 1)) && (type(b.get(sq)) == KNIGHT))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x - 1, y + 2)) && (type(b.get(sq)) == KNIGHT))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x - 1, y - 2)) && (type(b.get(sq)) == KNIGHT))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x - 2, y + 1)) && (type(b.get(sq)) == KNIGHT))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x - 2, y - 1)) && (type(b.get(sq)) == KNIGHT))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
        
    // kings
    if (val(sq = mksq(x + 1, y + 1)) && (type(b.get(sq)) == KING))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x + 1, y + 0)) && (type(b.get(sq)) == KING))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x + 1, y - 1)) && (type(b.get(sq)) == KING))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x + 0, y + 1)) && (type(b.get(sq)) == KING))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x + 0, y - 1)) && (type(b.get(sq)) == KING))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x - 1, y + 1)) && (type(b.get(sq)) == KING))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x - 1, y + 0)) && (type(b.get(sq)) == KING))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    if (val(sq = mksq(x - 1, y - 1)) && (type(b.get(sq)) == KING))
        count += (colour(b.get(sq)) == WHITE) ? 1 : -1;
    
    // pawns
    if (val(sq = mksq(x + 1, y + 1)) && (b.get(sq) == B_PAWN))
        count -= 1;
    if (val(sq = mksq(x - 1, y + 1)) && (b.get(sq) == B_PAWN))
        count -= 1;
    if (val(sq = mksq(x + 1, y - 1)) && (b.get(sq) == W_PAWN))
        count += 1;
    if (val(sq = mksq(x - 1, y - 1)) && (b.get(sq) == W_PAWN))
        count += 1;
        
    return count;
    
}

// print out a little grid of the control counts for each square
void display_control_counts(const Board & b) {

    
    for (int y = 7; y >= 0; --y) {
        for (int x = 0; x < 8; ++x) {
            int count = control_count(b, mksq(x, y));
            
            std::string sign;
            if (count > 0) { 
                sign = "+";
            } else if (count < 0) {
                sign = "";
            } else {
                sign = " ";
            }
            
            cout << sign << count << " ";
        }
        cout << "\n";
    }

}








