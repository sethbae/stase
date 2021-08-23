#include "game.h"
#include "../heur/heur.h"

int xd[8] = {-1, -1, 1, 1, -1, 1, 0, 0};
int yd[8] = {-1, 1, -1, 1, 0, 0, 1, -1};

int xkn[8] = {1, 1, 2, 2, -1, -1, -2, -2};
int ykn[8] = {2, -2, 1, -1, 2, -2, 1, -1};


/*
 * Walks along the board in the given manner, maintaining a running total of the +- control
 * of the given tempuare on the board. Updates min_threat if it encounters a threatening piece
 * of lower value than the current value of min_threat.
 */
int capture_walk(const Board & b, Square s) {
    
    int balance = 0;
    int min_value_w = 0;
    int min_value_b = 0;
    int x, y;
    Square temp;
    
    MoveType dir = DIAG;
    
    // go through the delta pairs entailing each sliding direction
    for (int i = 0; i < 8; ++i) {
        
        if (i == 4) {
            dir = ORTHO;
        }
        
        int x_inc = xd[i], y_inc = yd[i];
        bool cont = true;

        x = get_x(s), y = get_y(s);
        temp = mksq(x + x_inc, y + y_inc);
        
        // and go in that direction
        while (val(temp) && cont) {
    
            Piece p = b.get(mksq(x, y));
    
            if ((type(p) != EMPTY) && can_move(p, dir)) {
                // any piece which can move in the right dir: account and continue
                int val = piece_value(p);
                if (colour(p) == WHITE) {
                    ++balance;
                    if (val < min_value_w) {
                        min_value_w = val;
                    }
                } else {
                    --balance;
                    if (val < min_value_b) {
                        min_value_b = val;
                    }
                }
            } else  if (type(p) != EMPTY) {
                // blocking piece: abort
                cont = false;
            }
            
            x += x_inc;
            y += y_inc;
            
        }
        
    }
    
    x = get_x(s);
    y = get_y(s);

    // knights
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + xkn[i], y + ykn[i])) && (type(b.get(temp)) == KNIGHT)) {
            if (colour(b.get(temp)) == WHITE) {
                ++balance;
                if (min_value_w > 3) {
                    min_value_w = 3;
                }
            } else {
                --balance;
                if (min_value_b > 3) {
                    min_value_b = 3;
                }
            }
        }
    }

    // kings
    if (val(temp = mksq(x + 1, y + 1)) && (type(b.get(temp)) == KING))
        balance += (colour(b.get(temp)) == WHITE) ? 1 : -1;
    if (val(temp = mksq(x + 1, y + 0)) && (type(b.get(temp)) == KING))
        balance += (colour(b.get(temp)) == WHITE) ? 1 : -1;
    if (val(temp = mksq(x + 1, y - 1)) && (type(b.get(temp)) == KING))
        balance += (colour(b.get(temp)) == WHITE) ? 1 : -1;
    if (val(temp = mksq(x + 0, y + 1)) && (type(b.get(temp)) == KING))
        balance += (colour(b.get(temp)) == WHITE) ? 1 : -1;
    if (val(temp = mksq(x + 0, y - 1)) && (type(b.get(temp)) == KING))
        balance += (colour(b.get(temp)) == WHITE) ? 1 : -1;
    if (val(temp = mksq(x - 1, y + 1)) && (type(b.get(temp)) == KING))
        balance += (colour(b.get(temp)) == WHITE) ? 1 : -1;
    if (val(temp = mksq(x - 1, y + 0)) && (type(b.get(temp)) == KING))
        balance += (colour(b.get(temp)) == WHITE) ? 1 : -1;
    if (val(temp = mksq(x - 1, y - 1)) && (type(b.get(temp)) == KING))
        balance += (colour(b.get(temp)) == WHITE) ? 1 : -1;
    
    // pawns
    if (val(temp = mksq(x + 1, y + 1)) && (b.get(temp) == B_PAWN))
        balance -= 1;
    if (val(temp = mksq(x - 1, y + 1)) && (b.get(temp) == B_PAWN))
        balance -= 1;
    if (val(temp = mksq(x + 1, y - 1)) && (b.get(temp) == W_PAWN))
        balance += 1;
    if (val(temp = mksq(x - 1, y - 1)) && (b.get(temp) == W_PAWN))
        balance += 1;
    
    
    
}