#include <iostream>
using std::cout;

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
int capture_walk(const Board & b, Square s, int* min_w, int* min_b) {
    
    int balance = 0;
    int min_value_w = piece_value(KING)*10;
    int min_value_b = piece_value(KING)*10;
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
        bool x_ray = false;

        x = get_x(s) + x_inc, y = get_y(s) + y_inc;

        // and go in that direction
        while (val(temp = mksq(x, y)) && cont) {
    
            Piece p = b.get(temp);

            if ((type(p) != EMPTY) && can_move(p, dir)) {
                // any piece which can move in the right dir: account and continue
                int val = piece_value(p);
                if (colour(p) == WHITE) {
                    ++balance;
                    if (!x_ray && val < min_value_w) {
                        min_value_w = val;
                    }
                } else {
                    --balance;
                    if (!x_ray && val < min_value_b) {
                        min_value_b = val;
                    }
                }
                x_ray = true;
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
    int kn_val = piece_value(W_KNIGHT);
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + xkn[i], y + ykn[i])) && (type(b.get(temp)) == KNIGHT)) {
            if (colour(b.get(temp)) == WHITE) {
                ++balance;
                if (min_value_w > kn_val) {
                    min_value_w = kn_val;
                }
            } else {
                --balance;
                if (min_value_b > kn_val) {
                    min_value_b = kn_val;
                }
            }
        }
    }

    // kings
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + xd[i], y + yd[i])) && (type(b.get(temp)) == KING)) {
            if (colour(b.get(temp)) == WHITE) {
                ++balance;
            } else {
                --balance;
            }
        }
    }

    // pawns
    int pawn_val = piece_value(W_PAWN);
    if (val(temp = mksq(x + 1, y + 1)) && (b.get(temp) == B_PAWN)) {
        balance -= 1;
        if (min_value_b > pawn_val) {
            min_value_b = pawn_val;
        }
    }
    if (val(temp = mksq(x - 1, y + 1)) && (b.get(temp) == B_PAWN)) {
        balance -= 1;
        if (min_value_b > pawn_val) {
            min_value_b = pawn_val;
        }
    }
    if (val(temp = mksq(x + 1, y - 1)) && (b.get(temp) == W_PAWN)) {
        balance += 1;
        if (min_value_w > pawn_val) {
            min_value_w = pawn_val;
        }
    }
    if (val(temp = mksq(x - 1, y - 1)) && (b.get(temp) == W_PAWN)) {
        balance += 1;
        if (min_value_w > pawn_val) {
            min_value_w = pawn_val;
        }
    }

    cout << "Balance: " << balance
            << "\nMin white: " << min_value_w
            << "\nMin black: " << min_value_b << "\n";

    *min_w = min_value_w;
    *min_b = min_value_b;
    return balance;
}