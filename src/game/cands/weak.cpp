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
        Ptype x_ray_colour = INVALID;

        x = get_x(s) + x_inc, y = get_y(s) + y_inc;

        // and go in that direction
        while (val(temp = mksq(x, y)) && cont) {
    
            Piece p = b.get(temp);

            if ((type(p) != EMPTY) && can_move_in_direction(p, dir)) {

                // once we're in an x-ray, only that colour can x-ray
                if (x_ray && colour(p) != x_ray_colour) { break; }

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
                x_ray_colour = colour(p);

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
 * - attacked by a piece of lower value
 * - attacked by a piece of equal value and not sufficiently defended
 * - attacked by any piece and not defended at all
 * - not sufficiently defended and attacked by a piece of lower value than the weakest defender
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

    bool
        totally_undefended,
        attacked_at_all,
        attacked_by_weaker,
        attacked_by_equal,
        under_defended;
    int
        weakest_attacker,
        weakest_defender;

    if (colour(gs.board.get(centre)) == WHITE) {

        totally_undefended = (ss.min_w > piece_value(W_KING));
        attacked_at_all = (ss.min_b <= piece_value(B_KING));
        attacked_by_weaker = (ss.min_b < piece_value(gs.board.get(centre)));
        attacked_by_equal = (ss.min_b == piece_value(gs.board.get(centre)));
        under_defended = (ss.balance < 0);

        weakest_attacker = ss.min_b;
        weakest_defender = ss.min_w;

    } else {

        totally_undefended = (ss.min_b > piece_value(B_KING));
        attacked_at_all = (ss.min_w <= piece_value(W_KING));
        attacked_by_weaker = (ss.min_w < piece_value(gs.board.get(centre)));
        attacked_by_equal = (ss.min_w == piece_value(gs.board.get(centre)));
        under_defended = (ss.balance > 0);

        weakest_attacker = ss.min_w;
        weakest_defender = ss.min_b;
    }

//    cout << "totally undefended: " << totally_undefended << "\n";
//    cout << "attacked at all: " << attacked_at_all << "\n";
//    cout << "attacked by weaker: " << attacked_by_weaker << "\n";
//    cout << "attacked by equal: " << attacked_by_equal << "\n";
//    cout << "under defended: " << under_defended << "\n";
//    cout << "weakest attacker: " << weakest_attacker << "\n";
//    cout << "weakest defender: " << weakest_defender << "\n";
//    cout << "balance: " << ss.balance << "\n";

    return (
            attacked_by_weaker
            || (attacked_by_equal && under_defended)
            || (attacked_at_all && totally_undefended)
            || (under_defended && (weakest_attacker < weakest_defender))
    );

}

/**
 * Detects squares on the board which contain weak pieces (of either colour). A weak piece is:
 * - attacked by a piece of lower value
 * - attacked by a piece of equal value and not sufficiently defended
 * - attacked by any piece and not defended at all
 * - not sufficiently defended and attacked by a piece of lower value than the weakest defender
 *
 * Records in conf_1 the value of the least valuable white piece attacking the square
 * Records in conf_2 the value of the least valuable black piece attacking the square
 *
 * @param b the board
 * @param centre the square to look at
 */
bool is_weak_square(const Gamestate & gs, const Square s) {
    FeatureFrame ff;
    return is_weak_square(gs, s, &ff);
}

/**
 * Calculates exactly the same thing as is_weak_square, but after the given move has
 * taken place.
 */
bool would_be_weak_after_move(const Gamestate & gs, const Square s, const Move m) {

    gs.board.sneak(m);
    bool weak = is_weak_square(gs, s);
    gs.board.unsneak(m);

    return weak;

}
