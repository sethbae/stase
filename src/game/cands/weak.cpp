#include <iostream>
using std::cout;

#include "game.h"
#include "cands.h"
#include "../heur/heur.h"

/*
 * Walks along the board in the given manner, maintaining a running total of the +- control
 * of the given square on the board. Updates min_w/min_b if it encounters a threatening piece
 * of the relevant colour of lower value than the current value of min_w/min_b.
 */
bool capture_walk(const Board & b, Square s, FeatureFrame * ff) {

    int balance = 0;
    int min_value_w = piece_value(W_KING)*10;
    int min_value_b = piece_value(W_KING)*10;
    int x, y;
    Square temp;

    if (type(b.get(s)) == EMPTY) { return false; }

    // go through the delta pairs entailing each sliding direction
    MoveType dir = DIAG;
    for (int i = 0; i < 8; ++i) {
        
        if (i == 4) {
            dir = ORTHO;
        }
        
        int x_inc = XD[i], y_inc = YD[i];
        bool cont = true;
        bool x_ray = false;

        x = get_x(s) + x_inc, y = get_y(s) + y_inc;

        // and go in that direction
        while (val(temp = mksq(x, y)) && cont) {
    
            Piece p = b.get(temp);

            if ((type(p) != EMPTY) && can_move_in_direction(p, dir)) {
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
        if (val(temp = mksq(x + XKN[i], y + YKN[i])) && (type(b.get(temp)) == KNIGHT)) {
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
    int k_val = piece_value(W_KING);
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + XD[i], y + YD[i])) && (type(b.get(temp)) == KING)) {
            if (colour(b.get(temp)) == WHITE) {
                ++balance;
                if (min_value_w > k_val) {
                    min_value_w = k_val;
                }
            } else {
                --balance;
                if (min_value_b > k_val) {
                    min_value_b = k_val;
                }
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

    // record the minimum black and white values
    ff->conf_1 = min_value_w;
    ff->conf_2 = min_value_b;

    //cout << "Balance: " << balance
    //     << "\nMin white: " << min_value_w
    //     << "\nMin black: " << min_value_b << "\n";

    if (colour(b.get(s)) == WHITE) {
        return (balance < 0 && min_value_b <= piece_value(B_KING)) || (min_value_b < piece_value(b.get(s)));
    } else {
        return (balance > 0 && min_value_w <= piece_value(W_KING)) || (min_value_w < piece_value(b.get(s)));
    }

}

/**
 * Detects squares on the board which contain weak pieces (of either colour). A weak piece is:
 * - a piece on a square which the enemy have more control over.
 * - a piece threatened by a less valuable piece.
 *
 * Records in conf_1 the value of the least valuable white piece attacking the square
 * Records in conf_2 the value of the least valuable black piece attacking the square
 *
 * @param b the board
 * @param centre the square to look at
 * @param ff the feature frame to record in, if true
 */
bool weak_hook(const Board & b, Square centre, FeatureFrame * ff) {
    return capture_walk(b, centre, ff);
}
