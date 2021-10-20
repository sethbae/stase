#include <iostream>
using std::cout;

#include "game.h"
#include "cands.h"
#include "../heur/heur.h"

/**
 * Used to pass information from capture_walk back to the following functions.
 */
struct SquareStatus {
    int balance = 0;
    int min_w = 0;
    int min_b = 0;
};

/**
 * Walks outward from the given piece, maintaining a running total of the +- control
 * of the given square on the board. Maintains min_w and min_b as the pieces of each
 * colour with minimum value that attack the given square.
 */
void capture_walk(const Board & b, Square s, SquareStatus & sq_status) {

    int balance = 0;
    int min_value_w = piece_value(W_KING)*10;
    int min_value_b = piece_value(W_KING)*10;
    int x, y;
    Square temp;

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

    // record the information
    sq_status.balance = balance;
    sq_status.min_w = min_value_w;
    sq_status.min_b = min_value_b;

    //cout << "Balance: " << balance
    //     << "\nMin white: " << min_value_w
    //     << "\nMin black: " << min_value_b << "\n";

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
bool is_weak_square(const Gamestate & gs, Square centre, FeatureFrame * ff) {

    if (type(gs.board.get(centre)) == EMPTY) { return false; }

    SquareStatus ss;
    capture_walk(gs.board, centre, ss);

    ff->conf_1 = ss.min_w;
    ff->conf_2 = ss.min_b;

    if (colour(gs.board.get(centre)) == WHITE) {
        return (ss.balance < 0 && ss.min_b <= piece_value(B_KING)) || (ss.min_b < piece_value(gs.board.get(centre)));
    } else {
        return (ss.balance > 0 && ss.min_w <= piece_value(W_KING)) || (ss.min_w < piece_value(gs.board.get(centre)));
    }
}

bool is_weak_square(const Gamestate & gs, const Square s) {

    if (type(gs.board.get(s)) == EMPTY) { return false; }

    SquareStatus ss;
    capture_walk(gs.board, s, ss);

    if (colour(gs.board.get(s)) == WHITE) {
        return (ss.balance < 0 && ss.min_b <= piece_value(B_KING)) || (ss.min_b < piece_value(gs.board.get(s)));
    } else {
        return (ss.balance > 0 && ss.min_w <= piece_value(W_KING)) || (ss.min_w < piece_value(gs.board.get(s)));
    }
}

/**
 * Checks whether a piece moving from [from] to [to] would then be on a weak square. The definition
 * of weak is as elsewhere (see is_weak_square above). This assumes that the piece on the from-square
 * can move to the to-square.
 */
bool would_be_weak_square(const Gamestate & gs, const Square from, const Square to) {

    SquareStatus ss;
    capture_walk(gs.board, to, ss);

    // the piece on from controls the to square (because it can presumably move there), so we compensate for that
    // by adjusting the balance, before applying the same logic as in is_weak_square

    if (colour(gs.board.get(from)) == WHITE) {
        --ss.balance;
        return (ss.balance < 0 && ss.min_b <= piece_value(B_KING)) || (ss.min_b < piece_value(gs.board.get(from)));
    } else {
        ++ss.balance;
        return (ss.balance > 0 && ss.min_w <= piece_value(W_KING)) || (ss.min_w < piece_value(gs.board.get(from)));
    }
}

bool would_be_weak_if_attacked(const Gamestate & gs, const Square s, const Piece attacked_by) {

    if (type(gs.board.get(s)) == EMPTY) { return false; }

    SquareStatus ss;
    capture_walk(gs.board, s, ss);

    // the piece on from controls the to square (because it can presumably move there), so we compensate for that
    // by adjusting the balance, before applying the same logic as in is_weak_square

    if (colour(gs.board.get(s)) == WHITE) {
        --ss.balance;
        int min_attacker = ((piece_value(attacked_by) < ss.min_b) ? piece_value(attacked_by) : ss.min_b);
        return (ss.balance < 0) || (min_attacker < piece_value(gs.board.get(s)));
    } else {
        ++ss.balance;
        int min_attacker = ((piece_value(attacked_by) < ss.min_w) ? piece_value(attacked_by) : ss.min_w);
        return (ss.balance > 0) || (min_attacker < piece_value(gs.board.get(s)));
    }
}
