#include <iostream>
using std::cout;

#include "game.h"
#include "board.h"
#include "../heur/heur.h"
#include "glogic.h"
#include "../gamestate.hpp"

/**
 * Walks outward from the given piece, maintaining a running total of the +- control
 * of the given square on the board. Maintains min_w and min_b as the pieces of each
 * colour with minimum value that attack the given square.
 * Includes more complicated logic regarding x-rays including those of mixed colour.
 * @return a summary of the information found, a SquareControlStatus.
 */
SquareControlStatus evaluate_square_control(const Gamestate & gs, Square s) {

    /*
     *                  X-RAYS and POLY X-RAYS
     *
     * An x-ray is when multiple pieces able to move in a common direction
     * line up to control a square, eg ...RR..p
     *
     * A poly x-ray is when these pieces are of mixed colours, eg:
     *                  ..qQ...n
     *
     * For simple x-rays, it is okay to keep adding to the balance: they act
     * as one. For poly x-rays this is more complicated, however. Consider this
     * case:
     *          . . . . . . . k
     *          . . . q . R p .
     *          . . . . . . Q p
     * (fen "4r2k/3q1Rp1/6Qp/p3p3/8/2P4P/6PK/8 b - - 3 37")
     *
     * The black queen's defence of g7 is blocked by the white rook. It doesn't defend
     * g7 because white can take with his queen, and the black queen cannot recapture.
     * Suppose that the queen was on f5 however, and now the black queen could take the
     * rook after the (inadvisable) Rxg7.
     *
     * The possibly flawed approach of this implementation is to count poly x-ray balances
     * and weakest defenders separately. If the attacking side controls the square from
     * multiple directions, then the poly x-rays are ignored. If the attackers attack only
     * from one direction however, then poly x-rays affect the balance and min-defender.
     */

    int16_t basic_balance = 0;
    int16_t poly_x_ray_balance = 0;
    int directions_attacked_from = 0;
    bool attacked_by_pinned_w_piece = false;
    bool attacked_by_pinned_b_piece = false;
    uint16_t min_value_w = NOT_ATTACKED_AT_ALL;
    uint16_t min_value_b = NOT_ATTACKED_AT_ALL;
    uint16_t min_poly_x_ray_defender = NOT_ATTACKED_AT_ALL;
    int x, y;
    Square temp;

    // go through the delta pairs entailing each sliding direction
    MoveType dir = DIAG;
    for (int i = 0; i < 8; ++i) {

        if (i == 4) {
            dir = ORTHO;
        }

        // used to check if extra defenders are discovered in this direction
        int prior_balance = basic_balance;

        int x_inc = XD[i], y_inc = YD[i];
        bool cont = true;
        bool x_ray = false;
        Colour x_ray_colour = INVALID_COLOUR;
        bool poly_x_ray = false;

        x = get_x(s) + x_inc, y = get_y(s) + y_inc;

        // work outwards in that direction
        while (val(temp = mksq(x, y)) && cont) {

            Piece p = gs.board.get(temp);
            if (p == EMPTY) {
                x += x_inc;
                y += y_inc;
                continue;
            }

            bool moves_in_right_dir = can_move_in_direction(p, dir);

            if (!moves_in_right_dir && type(p) == PAWN && dir == DIAG) {

                // pawns need special care

                // they can only contribute from the very first square along the diagonal, because that's
                // the only square they can capture the target from
                if (x == get_x(s) + x_inc && y == get_y(s) + y_inc) {

                    // the diagonal has to be going in the right direction for their colour

                    if (colour(p) == WHITE && y_inc == -1) {
                        // white pawns can capture up the board, so if we are working out from
                        // the target and moving down the board, a white pawn is able to take
                        moves_in_right_dir = true;
                    } else if (colour(p) == BLACK && y_inc == 1) {
                        // vice versa
                        moves_in_right_dir = true;
                    }

                }
            }

            if (moves_in_right_dir) {

                if (gs.is_kpinned_piece(temp, delta(x_inc, y_inc))) {
                    cont = false;
                    if (colour(p) == WHITE) {
                        attacked_by_pinned_w_piece = true;
                    } else {
                        attacked_by_pinned_b_piece = true;
                    }
                    continue;
                }

                // attacking piece found: update (poly) x-ray info and balances

                if (x_ray && colour(p) != x_ray_colour) {
                    poly_x_ray = true;
                }

                int val = piece_value(p);
                if (colour(p) == WHITE) {
                    if (poly_x_ray) {
                        ++poly_x_ray_balance;
                    } else {
                        ++basic_balance;
                        ++poly_x_ray_balance;
                        if (val < min_value_w) {
                            min_value_w = val;
                        }
                    }
                } else {
                    if (poly_x_ray) {
                        --poly_x_ray_balance;
                    } else {
                        --basic_balance;
                        --poly_x_ray_balance;
                        if (val < min_value_b) {
                            min_value_b = val;
                        }
                    }
                }

                // is this piece a poly-x-ray defender?
                if (poly_x_ray
                    && (colour(p) == colour(gs.board.get(s)))
                    && piece_value(p) < min_poly_x_ray_defender) {
                    min_poly_x_ray_defender = piece_value(p);
                }

                x_ray = true;
                x_ray_colour = colour(p);

            } else {
                // blocking piece: abort
                cont = false;
            }

            x += x_inc;
            y += y_inc;

        }

        if (basic_balance != prior_balance) {
            // some defender was found in this direction
            ++directions_attacked_from;
        }

    }

    x = get_x(s);
    y = get_y(s);

    // knights
    int kn_val = piece_value(W_KNIGHT);
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + XKN[i], y + YKN[i]))
                && (type(gs.board.get(temp)) == KNIGHT)) {

            // if the piece is pinned, note this
            if (gs.is_kpinned_piece(temp, KNIGHT_DELTA)) {
                if (colour(gs.board.get(temp)) == WHITE) {
                    attacked_by_pinned_w_piece = true;
                } else {
                    attacked_by_pinned_b_piece = true;
                }
                continue;
            }

            // unpinned, account as usual
            if (colour(gs.board.get(temp)) == WHITE) {
                ++basic_balance;
                ++poly_x_ray_balance;
                if (min_value_w > kn_val) {
                    min_value_w = kn_val;
                }
            } else {
                --basic_balance;
                --poly_x_ray_balance;
                if (min_value_b > kn_val) {
                    min_value_b = kn_val;
                }
            }
            ++directions_attacked_from;
        }
    }

    // kings
    int k_val = piece_value(W_KING);
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + XD[i], y + YD[i])) && (type(gs.board.get(temp)) == KING)) {
            if (colour(gs.board.get(temp)) == WHITE) {
                ++basic_balance;
                ++poly_x_ray_balance;
                if (min_value_w > k_val) {
                    min_value_w = k_val;
                }
            } else {
                --basic_balance;
                --poly_x_ray_balance;
                if (min_value_b > k_val) {
                    min_value_b = k_val;
                }
            }
            ++directions_attacked_from;
        }
    }

    // update the min_values correctly
    if (min_value_w > piece_value(W_KING)) {
        if (attacked_by_pinned_w_piece) {
            min_value_w = ATTACKED_BY_PINNED_PIECE;
        } else {
            min_value_w = NOT_ATTACKED_AT_ALL;
        }
    }
    if (min_value_b > piece_value(B_KING)) {
        if (attacked_by_pinned_b_piece) {
            min_value_b = ATTACKED_BY_PINNED_PIECE;
        } else {
            min_value_b = NOT_ATTACKED_AT_ALL;
        }
    }

//    cout << "attacked from " << directions_attacked_from << " directions\n";
//    cout << "basic balance: " << basic_balance << "\n";
//    cout << "poly_x_ray_balance: " << poly_x_ray_balance << "\n";

    // if we attack from only one direction, poly x rays count, because they act in the only direction
    // in which we can capture; e.g. ...q.Q.n (the knight is not weak)
    // if we attack from multiple directions, however, then a piece from another direction can capture,
    // so the piece should be weak.
    if (directions_attacked_from > 1) {

        // attacking from multiple directions, poly x-rays do not count
        return {
            basic_balance,
            min_value_w,
            min_value_b
        };

    } else {

        // attacking from one route only, poly x-rays do count

        // we also need to check for poly x-ray pieces that are the only or weakest defender
        if (colour(gs.board.get(s)) == WHITE) {

            // if the weak piece is white, its defenders are white
            uint16_t min_defender = (min_poly_x_ray_defender < min_value_w) ? min_poly_x_ray_defender : min_value_w;

            return {
              poly_x_ray_balance,
              min_defender,
              min_value_b
            };

        } else {

            // if the weak piece is black, its defenders are black
            uint16_t min_defender = (min_poly_x_ray_defender < min_value_b) ? min_poly_x_ray_defender : min_value_b;

            return {
                poly_x_ray_balance,
                min_value_w,
                min_defender
            };
        }

    }

}

/**
 * Analyses a SquareControlStatus as returned by evaluate_square_control to detect whether the square is in fact
 * a weak square. If a square is empty, a weak square is defined as one whose control count is strictly
 * unfavourable (ie not zero) for the given colour [c].
 * If a square is not empty, then weakness is more complex:
 * - attacked by a piece of lower value
 * - attacked by a piece of equal value and not sufficiently defended
 * - attacked by any piece and not defended at all
 * - not sufficiently defended and attacked by a piece of lower value than the weakest defender
 */
bool is_weak_status(const Gamestate & gs, const Square s, const Colour c, SquareControlStatus ss) {

    if (gs.board.get(s) == EMPTY) {
        return c == WHITE
               ? ss.balance < 0
               : ss.balance > 0;
    }

    bool
            totally_undefended,
            attacked_at_all,
            attacked_by_weaker,
            attacked_by_equal,
            under_defended;
    int
            weakest_attacker,
            weakest_defender;

    if (colour(gs.board.get(s)) == WHITE) {

        totally_undefended = (ss.min_w > piece_value(W_KING));
        attacked_at_all = (ss.min_b <= piece_value(B_KING));
        attacked_by_weaker = (ss.min_b < piece_value(gs.board.get(s)));
        attacked_by_equal = (ss.min_b == piece_value(gs.board.get(s)));
        under_defended = (ss.balance < 0);

        weakest_attacker = ss.min_b;
        weakest_defender = ss.min_w;

    } else {

        totally_undefended = (ss.min_b > piece_value(B_KING));
        attacked_at_all = (ss.min_w <= piece_value(W_KING));
        attacked_by_weaker = (ss.min_w < piece_value(gs.board.get(s)));
        attacked_by_equal = (ss.min_w == piece_value(gs.board.get(s)));
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
//    cout << "basic_balance: " << ss.balance << "\n";

    return (
            attacked_by_weaker
            || (attacked_by_equal && under_defended)
            || (attacked_at_all && totally_undefended)
            || (under_defended && (weakest_attacker <= weakest_defender))
    );

}

/**
 * Checks if the given square is weak for either white or black (according to the given colour [c]).
 * If a square is empty, then only the control count is looked at, which must be strictly less or more
 * than zero respectively.
 * If a square is not empty, there is a more complex set of rules. In this case, the colour is not
 * relevant and the return value represents solely the weakness/safety of the piece on the square.
 * The definition of weakness is then:
 * - attacked by a piece of lower value
 * - attacked by a piece of equal value and not sufficiently defended
 * - attacked by any piece and not defended at all
 * - not sufficiently defended and attacked by a piece of lower value than the weakest defender
 * However, there is more complicated logic regarding the interaction of x-rays when pieces are
 * on the same diagonal/orthogonal line.
 */
bool is_weak_square(const Gamestate & gs, const Square s, const Colour c, const bool use_caches) {
    if (!use_caches) {
        return is_weak_status(gs, s, c, evaluate_square_control(gs, s));
    }
    return is_weak_status(gs, s, c, gs.control_cache->get_control_status(s));
}

/**
 * Calculates exactly the same thing as is_weak_square, but after the given move has
 * taken place.
 * Not suitable for kings (use eg is_safe_for_king).
 */
bool would_be_weak_after(const Gamestate & gs, const Square s, const Colour c, const Move m) {

    Piece sneaked = gs.sneak(m);
    bool weak = is_weak_square(gs, s, c, false);
    gs.unsneak(m, sneaked);

    return weak;

}

/**
 * Returns true iff the given square has a weak piece on it. For the definition of weak,
 * see is_weak_square.
 * Not suitable for kings (use eg is_safe_for_king).
 */
bool is_unsafe_piece(const Gamestate & gs, const Square s) {
    return (gs.board.get(s) != EMPTY) && is_weak_square(gs, s, colour(gs.board.get(s)));
}

/**
 * Returns the same as is_unsafe_piece, but after the given move has been played.
 * Not suitable for kings (use eg is_safe_for_king).
 */
bool would_be_unsafe_after(const Gamestate & gs, const Square s, const Move m) {
    return (gs.board.get(s) != EMPTY || equal(s, m.to)) && would_be_weak_after(gs, s, colour(gs.board.get(s)), m);
}

/**
 * Returns false if the move moves a piece onto a square which it isn't safe on. If the square
 * would be safe, returns true.
 */
bool move_is_safe(const Gamestate & gs, const Move m) {

    Piece p = gs.board.get(m.from);
    if (p == EMPTY) { return true; }

    SquareControlStatus status = gs.control_cache->get_control_status(m.to);

    if (colour(p) == WHITE) {

        if (type(p) != PAWN) {
            // pawns don't control the squares they move onto
            status.balance -= 1;
        }

        // if the square is not controlled, the move is safe.
        if (status.min_b > piece_value(B_KING)) { return true; }

        // if it's attacked only by more valuable pieces, then:
        if (status.min_b > piece_value(p)) {
            // uncontrolled square: unsafe
            if (status.balance < 0) { return false; }
            // neutral square: safe if defended at all
            if (status.balance == 0) { return status.min_w <= piece_value(W_KING); }
            // controlled square: safe
            return true;
        }

        // if attacked by same piece, then:
        else if (status.min_b == piece_value(p)) {
            // safe unless the square is definitely not controlled
            return status.balance >= 0;
        }

        // if attacked by weaker piece, then unsafe
        else {
            return false;
        }

    } else {

        if (type(p) != PAWN) {
            // pawns don't control the squares they move onto
            status.balance += 1;
        }

        // if the square is not controlled, the move is safe.
        if (status.min_w > piece_value(W_KING)) { return true; }

        // if it's attacked only by more valuable pieces, then:
        if (status.min_w > piece_value(p)) {
            // uncontrolled square: unsafe
            if (status.balance > 0) { return false; }
            // neutral square: safe if defended at all
            if (status.balance == 0) { return status.min_b <= piece_value(B_KING); }
            // controlled square: safe
            return true;
        }

        // if attacked by same piece, then:
        else if (status.min_w == piece_value(p)) {
            // safe unless the square is definitely not controlled
            return status.balance <= 0;
        }

        // if attacked by weaker piece, then unsafe
        else {
            return false;
        }
    }
}
