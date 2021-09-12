#include "game.h"
#include "board.h"

#include <iostream>
using std::cout;

StepFunc *STEP_FUNCS[] = {
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

/*
 * Performs a linear search on the board according to the given step and val functions.
 * returns the alpha count of the walk
 */
inline int alpha_walk(const Board & b, const Square s, StepFunc *step) {
    
    int sum = 0;
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

/*
 * computes and returns the number of squares which the given piece 'alpha controls'
 */
int alpha_control(const Board & b, const Square s) {
    
    int sum = 0;
    
    Piece p = b.get(s);
    
    if (can_move_in_direction(p, ORTHO)) {
        for (int dir = ORTHO_START; dir < ORTHO_STOP; ++dir) {
            sum += alpha_walk(b, s, STEP_FUNCS[dir]);
        }
    }
    
    if (can_move_in_direction(p, DIAG)) {
        for (int dir = DIAG_START; dir < DIAG_STOP; ++dir) {
            sum += alpha_walk(b, s, STEP_FUNCS[dir]);
        }
    }
    
    if (can_move_in_direction(p, KNIGHT_MOVE)) {
    
        int x = get_x(s);
        int y = get_y(s);
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
inline int beta_walk(const Board & b, const Square s, StepFunc *step) {
    
    int sum = 0;
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
int beta_control(const Board & b, const Square s) {
    
    int sum = 0;
    
    Piece p = b.get(s);
    
    if (can_move_in_direction(p, ORTHO)) {
        for (int dir = ORTHO_START; dir < ORTHO_STOP; ++dir) {
            sum += beta_walk(b, s, STEP_FUNCS[dir]);
        }
    }
    
    if (can_move_in_direction(p, DIAG)) {
        for (int dir = DIAG_START; dir < DIAG_STOP; ++dir) {
            sum += beta_walk(b, s, STEP_FUNCS[dir]);
        }
    }
    
    if (can_move_in_direction(p, KNIGHT_MOVE)) {
    
        int x = get_x(s);
        int y = get_y(s);
        
        if (val(mksq(x + 1, y + 2)))
            ++sum;
        if (val(mksq(x + 1, y - 2)))
            ++sum;
        if (val(mksq(x + 2, y + 1)))
            ++sum;
        if (val(mksq(x + 2, y - 1)))
            ++sum;
        if (val(mksq(x - 1, y + 2)))
            ++sum;
        if (val(mksq(x - 1, y - 2)))
            ++sum;
        if (val(mksq(x - 2, y + 1)))
            ++sum;
        if (val(mksq(x - 2, y - 1)))
            ++sum;
        
    }
    
    return sum;

}

// performs a linear search on the board according to the given step and val functions.
// returns the gamma count of the walk
inline int gamma_walk(const Board & b, const Square s, StepFunc *step, MoveType dir) {
    
    int sum = 0;
    Square temp = s;
    bool cont = true;
    
    (*step)(temp);
    
    while (val(temp) && cont) {

        Piece otherp = b.get(temp);
        if (type(otherp) == EMPTY) {
            ++sum;
        } else {
            // gamma: x-ray
            cont = (colour(otherp) == colour(b.get(s))) && can_move_in_direction(otherp, dir);
            ++sum;
        }
        (*step)(temp);
    }
    
    return sum;
}

// computes and returns the number of squares which the given piece 'gamma controls'
int gamma_control(const Board & b, const Square s) {
    
    int sum = 0;
    
    Piece p = b.get(s);
    
    if (can_move_in_direction(p, ORTHO)) {
        for (int dir = ORTHO_START; dir < ORTHO_STOP; ++dir) {
            sum += gamma_walk(b, s, STEP_FUNCS[dir], ORTHO);
        }
    }
    
    if (can_move_in_direction(p, DIAG)) {
        for (int dir = DIAG_START; dir < DIAG_STOP; ++dir) {
            sum += gamma_walk(b, s, STEP_FUNCS[dir], DIAG);
        }
    }
    
    if (can_move_in_direction(p, KNIGHT_MOVE)) {
    
        int x = get_x(s);
        int y = get_y(s);

        if (val(mksq(x + 1, y + 2)))
            ++sum;
        if (val(mksq(x + 1, y - 2)))
            ++sum;
        if (val(mksq(x + 2, y + 1)))
            ++sum;
        if (val(mksq(x + 2, y - 1)))
            ++sum;
        if (val(mksq(x - 1, y + 2)))
            ++sum;
        if (val(mksq(x - 1, y - 2)))
            ++sum;
        if (val(mksq(x - 2, y + 1)))
            ++sum;
        if (val(mksq(x - 2, y - 1)))
            ++sum;
    }
    
    return sum;

}


/*
    walks outward from the given square. Returns an integer representing the resulting control
    change from that walk. 
    So if it first encounters a white piece that can move in the right way, it goes +1 but 
    continues looking for a different piece along the same diagonal which may be able to x-ray
    control the square.
    
    This method ignores pawns and kings.
*/
int control_walk(const Board & b, const Square s, StepFunc *step, MoveType dir) {

    int sum = 0;
    Square temp = s;
    bool cont = true;
    
    (*step)(temp);
    
    while (val(temp) && cont) {

        Piece otherp = b.get(temp);

        if ((type(otherp) != EMPTY) && can_move_in_direction(otherp, dir)) {
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
    for (int dir = ORTHO_START; dir < ORTHO_STOP; ++dir) {
        count += control_walk(b, s, STEP_FUNCS[dir], ORTHO);
    }

    // diagonal movement
    for (int dir = DIAG_START; dir < DIAG_STOP; ++dir) {
        count += control_walk(b, s, STEP_FUNCS[dir], DIAG);
    }
    
    
    int x = get_x(s);
    int y = get_y(s);
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

/*
 * Returns true if the piece currently on the from-square could move to the to-square,
 * disregarding check and other considerations.
 */
bool can_move_to_square(const Board & b, Square from_sq, Square to_sq) {
    std::vector<Move> pmoves(10);
    piecemoves_ignore_check(b, from_sq, pmoves);
    for (const Move m : pmoves) {
        if (m.to == to_sq) {
            return true;
        }
    }
    return false;
}

bool alpha_cover_slide(const Board & b, Square piece_sq, Square target_sq) {

    const Piece p = b.get(piece_sq);
    int xd = get_x(target_sq) - get_x(piece_sq);
    int yd = get_y(target_sq) - get_y(piece_sq);
    const bool ortho = (xd == 0) || (yd == 0);
    const bool diag = (xd + yd == 0) || (xd - yd == 0);

    if (xd == 0 && yd == 0) { return false; }
    if (!ortho && !diag) { return false; }
    if (type(p) == ROOK && diag) { return false; }
    if (type(p) == BISHOP && ortho) { return false; }

    // convert the actual difference to +-1
    if (xd) { xd /= abs(xd); }
    if (yd) { yd /= abs(yd); }

    Square temp;
    int x = get_x(piece_sq) + xd, y = get_y(piece_sq) + yd;

    while (val(temp = mksq(x, y)) && temp != target_sq) {

        const Piece otherp = b.get(temp);

        if (otherp != EMPTY) {
            return false;
        }

        x += xd;
        y += yd;

    }

    // loop continued up to the target square - check that square itself!
    return val(temp) && b.get(target_sq) == EMPTY;

}

/*
 * A piece alpha covers all empty squares it can move to.
 */
bool alpha_covers(const Board & b, Square piece_sq, Square target_sq) {
    switch (type(b.get(piece_sq))) {
        case QUEEN:
        case BISHOP:
        case ROOK:
            return alpha_cover_slide(b, piece_sq, target_sq);
        default:
            return false;
    }
}

bool beta_cover_slide(const Board & b, Square piece_sq, Square target_sq) {

    const Piece p = b.get(piece_sq);
    int xd = get_x(target_sq) - get_x(piece_sq);
    int yd = get_y(target_sq) - get_y(piece_sq);
    const bool ortho = (xd == 0) || (yd == 0);
    const bool diag = (xd + yd == 0) || (xd - yd == 0);

    if (xd == 0 && yd == 0) { return false; }
    if (!ortho && !diag) { return false; }
    if (type(p) == ROOK && diag) { return false; }
    if (type(p) == BISHOP && ortho) { return false; }

    // convert the actual difference to +-1
    if (xd) { xd /= abs(xd); }
    if (yd) { yd /= abs(yd); }

    Square temp;
    int x = get_x(piece_sq) + xd, y = get_y(piece_sq) + yd;

    while (val(temp = mksq(x, y)) && temp != target_sq) {

        const Piece otherp = b.get(temp);

        if (otherp != EMPTY) {
            return false;
        }

        x += xd;
        y += yd;

    }

    return val(temp);

}

/*
 * A piece beta-covers the squares it can move to, as well as those immediately following
 * which have a piece on them.
 */
bool beta_covers(const Board & b, Square piece_sq, Square target_sq) {
    switch (type(b.get(piece_sq))) {
        case QUEEN:
        case BISHOP:
        case ROOK:
            return beta_cover_slide(b, piece_sq, target_sq);
        default:
            return false;
    }
}

bool gamma_cover_slide(const Board & b, Square piece_sq, Square target_sq) {

    const Piece p = b.get(piece_sq);
    int xd = get_x(target_sq) - get_x(piece_sq);
    int yd = get_y(target_sq) - get_y(piece_sq);
    const bool ortho = (xd == 0) || (yd == 0);
    const bool diag = (xd + yd == 0) || (xd - yd == 0);
    const MoveType dir = ortho ? ORTHO : DIAG;

    if (xd == 0 && yd == 0) { return false; }
    if (!ortho && !diag) { return false; }
    if (type(p) == ROOK && diag) { return false; }
    if (type(p) == BISHOP && ortho) { return false; }

    // convert the actual difference to +-1
    if (xd) { xd /= abs(xd); }
    if (yd) { yd /= abs(yd); }

    Square temp;
    int x = get_x(piece_sq) + xd, y = get_y(piece_sq) + yd;

    while (val(temp = mksq(x, y)) && temp != target_sq) {

        const Piece otherp = b.get(temp);

        if (otherp != EMPTY && !can_move_in_direction(otherp, dir)) {
            return false;
        }

        x += xd;
        y += yd;

    }

    return val(temp);

}

/*
 * A piece gamma controls all beta control squares, extending its reach through
 * any non-empty squares whose pieces can also move in the same direction.
 */
bool gamma_covers(const Board & b, Square piece_sq, Square target_sq) {
    switch (type(b.get(piece_sq))) {
        case QUEEN:
        case BISHOP:
        case ROOK:
            return gamma_cover_slide(b, piece_sq, target_sq);
        default:
            return false;
    }
}


/*
 * Below code was a start on defining these for non-sliding squares, which turns
 * out to be kinda complicated and not immediately useful?
 */

//bool knight_can_move_to(const Board & b, Square piece_sq, Square target_sq) {
//    int x = get_x(piece_sq), y = get_y(piece_sq);
//    Square temp;
//    for (int i = 0; i < 8; ++i) {
//        if (val(temp = mksq(x + XKN[i], y + YKN[i])) && temp == target_sq) {
//            return true;
//        }
//    }
//    return false;
//}
//
//bool king_can_move_to(const Board & b, Square piece_sq, Square target_sq) {
//    int x = get_x(piece_sq), y = get_y(piece_sq);
//    Square temp;
//    for (int i = 0; i < 8; ++i) {
//        if (val(temp = mksq(x + XD[i], y + YD[i])) && temp == target_sq) {
//            return true;
//        }
//    }
//    return false;
//}
//
//bool pawn_can_move_to(const Board & b, Square piece_sq, Square target_sq) {
//    int x = get_x(piece_sq), y = get_y(piece_sq);
//    Square temp;
//    if (colour(b.get(piece_sq)) == WHITE) {
//        if (b.get(temp = mksq(x, y + 1)) != EMPTY) { return false; }
//        if (temp == target_sq) { return true; }
//        if (get_y(piece_sq) == 1) {
//            return b.get(temp = mksq(x, y + 2)) == EMPTY && temp == target_sq;
//        }
//    } else {
//        if (b.get(temp = mksq(x, y - 1)) != EMPTY) { return false; }
//        if (temp == target_sq) { return true; }
//        if (get_y(piece_sq) == 6) {
//            return b.get(temp = mksq(x, y - 2)) == EMPTY && temp == target_sq;
//        }
//    }
//    return false;
//}
