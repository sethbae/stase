#include <iostream>
using std::cout;

#include "board.h"
#include "../heur/heur.h"
#include "glogic.h"
#include "../gamestate.hpp"

/**
 * Returns true if the piece on the square s is the piece which separates colour control
 * along a poly x-ray line. For example:
 *              --r-QR-x-
 * Here, the rook controls x by a poly x-ray, and the queen is the white piece which
 * 'separates' the white and black pieces. The white rook, however, does not separate
 * them, because the white queen comes between it and the first black piece found.
 */
bool is_separator_on_poly_x_ray(const Gamestate & gs, const Square s, const Delta d) {

    int x = s.x + d.dx, y = s.y + d.dy;
    bool cont = true;

    while (val(x, y) && cont) {

        Piece p = gs.board.get(x, y);

        if (p == EMPTY) {
            x += d.dx;
            y += d.dy;
            continue;
        }
        if (!can_move_in_direction(p, d)) {
            cont = false;
            continue;
        }

        // we've found the first piece behind this one; check its colour
        return colour(p) != colour(gs.board.get(s));
    }

    return false;
}

/**
 * Finds the pieces of minimum value which control the given square. Records the value of
 * the minimum white piece in min_w and vice versa for min_b.
 */
void find_min_attackers(const Gamestate & gs, Square s, uint16_t * min_w, uint16_t * min_b) {

    /*
     * Throughout, early exits are possible because pieces are tackled in non-decreasing order.
     */

    //TODO: try separating this into two functions for b/w to see the speedup

    //TODO: write some tests!

    Square temp;
    *min_w = piece_value(W_KING) + 1;
    *min_b = piece_value(W_KING) + 1;

    // pawns
    if (val(temp = mksq(s.x - 1, s.y - 1)) && gs.board.get(temp) == W_PAWN && !gs.is_kpinned_piece(temp, delta(-1, -1))) {
        *min_w = piece_value(W_PAWN);
    } else if (val(temp = mksq(s.x + 1, s.y - 1)) && gs.board.get(temp) == W_PAWN && !gs.is_kpinned_piece(temp, delta(+1, -1))) {
        *min_w = piece_value(W_PAWN);
    }
    if (val(temp = mksq(s.x - 1, s.y + 1)) && gs.board.get(temp) == B_PAWN && !gs.is_kpinned_piece(temp, delta(-1, +1))) {
        *min_b = piece_value(B_PAWN);
    } else if (val(temp = mksq(s.x + 1, s.y + 1)) && gs.board.get(temp) == B_PAWN && !gs.is_kpinned_piece(temp, delta(+1, +1))) {
        *min_b = piece_value(B_PAWN);
    }

    if (*min_w <= piece_value(W_KING) && *min_b <= piece_value(W_KING)) { return; }

    // knights
    for (int i = 0; i < 8; ++i) {

        temp = mksq(s.x + XKN[i], s.y + YKN[i]);
        Piece p = gs.board.get(temp);

        if (p == W_KNIGHT) {
            if (!gs.is_kpinned_piece(temp, KNIGHT_DELTA)) {
                *min_w = piece_value(W_KNIGHT);
            }
        } else if (p == B_KNIGHT) {
            if (!gs.is_kpinned_piece(temp, KNIGHT_DELTA)) {
                *min_b = piece_value(W_KNIGHT);
            }
        }
    }

    if (*min_w <= piece_value(W_KING) && *min_b <= piece_value(W_KING)) { return; }

    // sliding pieces
    MoveType dir = DIAG;
    for (int i = 0; i < 8; ++i) {

        if (i == 4) { dir = ORTHO; }

        Delta d{XD[i], YD[i]};
        temp = gs.first_piece_encountered(s, d);

//        std::cout << ptoc(gs.board.get(temp)) << "\n";

        if (val(temp)) {
            Piece p = gs.board.get(gs.first_piece_encountered(s, d));
            if (can_move_in_direction(p, dir) && !gs.is_kpinned_piece(temp, d)) {
                if (colour(p) == WHITE) {
                    if (piece_value(p) < *min_w) {
                        *min_w = piece_value(p);
                    }
                } else {
                    if (piece_value(p) < *min_b) {
                        *min_b = piece_value(p);
                    }
                }
            }
        }
    }

    if (*min_w <= piece_value(W_KING) && *min_b <= piece_value(W_KING)) { return; }

    // kings
    for (int i = 0; i < 8; ++i) {

        if (!val(s.x + XD[i], s.y + YD[i])) { continue; }

        Piece p = gs.board.get(s.x + XD[i], s.y + YD[i]);
        if (p == W_KING) {
            *min_w = piece_value(W_KING);
        } else if (p == B_KING) {
            *min_b = piece_value(B_KING);
        }
    }

}

/**
 * Walks outward from the given piece, maintaining a running total of the +- control
 * of the given square on the board. Maintains min_w and min_b as the pieces of each
 * colour with minimum value that attack the given square.
 * Includes more complicated logic regarding x-rays including those of mixed colour.
 * @return a summary of the information found, a SquareControlStatus.
 */
SquareControlStatus capture_walk(const Gamestate & gs, Square s) {

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
    uint16_t min_value_w = piece_value(W_KING) + 1;
    uint16_t min_value_b = piece_value(W_KING) + 1;
    uint16_t min_poly_x_ray_defender = piece_value(W_KING) + 1;
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
            if (gs.is_kpinned_piece(temp, delta(x_inc, y_inc))) {
                cont = false;
                continue;
            }

            bool moves_in_right_dir = (p != EMPTY) && can_move_in_direction(p, dir);

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

            } else  if (p != EMPTY) {
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
                && (type(gs.board.get(temp)) == KNIGHT)
                && !gs.is_kpinned_piece(temp, KNIGHT_DELTA)) {
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
 * This method retrieves the actual control status of a given square, without
 * applying any logic or interpretation to the result.
 */
SquareControlStatus evaluate_square_status(const Gamestate & gs, const Square s) {
    if (gs.control_cache->contains(s)) {
        return gs.control_cache->get(s);
    }
    SquareControlStatus status = capture_walk(gs, s);
    gs.control_cache->put(s, status);
    return status;
}

/**
 * Interpolates a new SquareControlStatus on the basis of the given one. This function assumes that
 * the move made is onto the square in question; it doesn't work otherwise. The from_sq should be
 * the square which the piece originates from.
 */
SquareControlStatus update_status(const Gamestate & gs, SquareControlStatus status, Square s, Square from_sq) {

    int16_t new_balance = status.balance;
    uint16_t new_min_w = status.min_w;
    uint16_t new_min_b = status.min_b;
    bool is_white_piece = colour(gs.board.get(from_sq)) == WHITE;

    if (is_white_piece) {
        --new_balance;
        if (is_separator_on_poly_x_ray(gs, from_sq, get_delta_between(s, from_sq))) {
            // if the piece was "separating" a poly x-ray, there is now no poly x-ray.
            // so on top of the fact this piece does not cover the original square,
            // there is also the fact that whichever pieces were blocked by the poly
            // x-ray are no longer blocked.
            --new_balance;
        }
    } else {
        ++new_balance;
        if (is_separator_on_poly_x_ray(gs, from_sq, get_delta_between(s, from_sq))) {
            ++new_balance;
        }
    }

    bool refresh_min_attackers = false;

    // quite a lot of caution is needed with refreshing the attackers: if either side did not attack
    // at all, you need to refresh; this is because of eg a poly x-ray.
    // otherwise, you only need to refresh if you actually could have been the min attacker.
    if (is_white_piece) {
        if (piece_value(gs.board.get(from_sq)) == status.min_w
                || status.min_w > piece_value(W_KING)
                || status.min_b > piece_value(W_KING)) {
            refresh_min_attackers = true;
        }
    } else {
        if (piece_value(gs.board.get(from_sq)) == status.min_b
                || status.min_b > piece_value(W_KING)
                || status.min_w > piece_value(W_KING)) {
            refresh_min_attackers = true;
        }
    }

    if (refresh_min_attackers) {
        Piece sneaked = gs.board.get(from_sq);
        gs.board.sneak_set(from_sq, EMPTY);
        find_min_attackers(gs, s, &new_min_w, &new_min_b);
        gs.board.sneak_set(from_sq, sneaked);
    }

    return SquareControlStatus{
        new_balance,
        new_min_w,
        new_min_b
    };

}

/**
 * Analyses a SquareControlStatus to determine whether it would be safe to move a piece
 * of the given colour onto a square with the given control status.
 */
bool is_weak_status_for_colour(const Gamestate & gs, const SquareControlStatus ss, const Colour c) {
    return c == WHITE
        ? ss.balance < 0
        : ss.balance > 0;
}

/**
 * Analyses a SquareControlStatus to detect whether the square is in fact a weak
 * square for the given piece type. This takes several factors into account, such as:
 * - attacked by a piece of lower value
 * - attacked by a piece of equal value and not sufficiently defended
 * - attacked by any piece and not defended at all
 * - not sufficiently defended and attacked by a piece of lower value than the weakest defender
 */
bool is_weak_status_for_piece(const Gamestate & gs, SquareControlStatus ss, const Piece p) {

    bool
            totally_undefended,
            attacked_at_all,
            attacked_by_weaker,
            attacked_by_equal,
            under_defended;
    int
            weakest_attacker,
            weakest_defender;

    if (colour(p) == WHITE) {

        totally_undefended = (ss.min_w > piece_value(W_KING));
        attacked_at_all = (ss.min_b <= piece_value(B_KING));
        attacked_by_weaker = (ss.min_b < piece_value(p));
        attacked_by_equal = (ss.min_b == piece_value(p));
        under_defended = (ss.balance < 0);

        weakest_attacker = ss.min_b;
        weakest_defender = ss.min_w;

    } else {

        totally_undefended = (ss.min_b > piece_value(B_KING));
        attacked_at_all = (ss.min_w <= piece_value(W_KING));
        attacked_by_weaker = (ss.min_w < piece_value(p));
        attacked_by_equal = (ss.min_w == piece_value(p));
        under_defended = (ss.balance > 0);

        weakest_attacker = ss.min_w;
        weakest_defender = ss.min_b;
    }

//    cout << "Colour: " << (colour(p) == WHITE ? "white\n" : "black\n");
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
 * Returns true iff the square is controlled by the opposing colour to that given.
 * So if WHITE was given, this returns true if BLACK control the square, and false
 * if WHITE control it or if the control is balanced.
 * No notice is taken of whether the square is empty or not: use weak_piece if you
 * want to know whether a piece on a given square is safe there or not.
 */
bool weak_square(const Gamestate & gs, const Square s, Colour c) {

    if (gs.control_cache->contains(s)) {
        return is_weak_status_for_colour(gs, gs.control_cache->get(s), c);
    }

    SquareControlStatus status = capture_walk(gs, s);
    gs.control_cache->put(s, status);

    return is_weak_status_for_colour(gs, status, c);

}

/**
 * Returns true iff the given square is not empty, and if the piece on it is not
 * safe to remain there.
 */
bool weak_piece(const Gamestate & gs, const Square s) {

    // empty squares by definition don't contain an unsafe piece
    if (gs.board.get(s) == EMPTY) { return false; }

    if (gs.control_cache->contains(s)) {
        return is_weak_status_for_piece(gs, gs.control_cache->get(s), gs.board.get(s));
    }

    SquareControlStatus status = capture_walk(gs, s);
    gs.control_cache->put(s, status);

    return is_weak_status_for_piece(gs, status, gs.board.get(s));
}

/**
 * This behaves as if the given move had been played.
 * Returns true iff the square is controlled by the opposing colour to that given.
 * So if WHITE was given, this returns true if BLACK control the square, and false
 * if WHITE control it or if the control is balanced.
 * No notice is taken of whether the square is empty or not: use weak_piece if you
 * want to know whether a piece on a given square is safe there or not.
 */
bool weak_square(const Gamestate & gs, const Square s, const Colour c, const Move m) {

    // we can only interpolate if (i) the move is onto the square in question (ii) it's neither king nor pawn
    if (equal(m.to, s) && type(gs.board.get(m.from)) != PAWN && type(gs.board.get(m.from)) != KING) {

        SquareControlStatus status;
        if (gs.control_cache->contains(s)) {
            status = gs.control_cache->get(s);
        } else {
            status = capture_walk(gs, s);
            gs.control_cache->put(s, status);
        }

        status = update_status(gs, status, s, m.from);

        return is_weak_status_for_colour(gs, status, c);

    } else {

        Piece sneaked = gs.board.sneak(m);
        SquareControlStatus status = capture_walk(gs, s);
        gs.board.unsneak(m, sneaked);

        return is_weak_status_for_colour(gs, status, c);
    }

}

/**
 * This behaves as if the given move had been played.
 * Returns true iff the given square is not empty, and if the piece on it is not
 * safe to remain there.
 */
bool weak_piece(const Gamestate & gs, const Square s, const Move m) {

    // empty squares by definition don't contain an unsafe piece
    if (!equal(m.to, s) && gs.board.get(s) == EMPTY) { return false; }

    // we can only interpolate if (i) the move is onto the square in question (ii) it's neither king nor pawn
    if (equal(m.to, s) && type(gs.board.get(m.from)) != PAWN && type(gs.board.get(m.from)) != KING) {

        SquareControlStatus status;
        if (gs.control_cache->contains(s)) {
            status = gs.control_cache->get(s);
        } else {
            status = capture_walk(gs, s);
            gs.control_cache->put(s, status);
        }

        status = update_status(gs, status, s, m.from);

        return is_weak_status_for_piece(gs, status, gs.board.get(m.from));

    } else {

        Piece sneaked = gs.board.sneak(m);
        SquareControlStatus status = capture_walk(gs, s);
        gs.board.unsneak(m, sneaked);

        if (equal(s, m.to)) {
            // king/pawn moves come out in this branch
            return is_weak_status_for_piece(gs, status, gs.board.get(m.from));
        } else {
            return is_weak_status_for_piece(gs, status, gs.board.get(s));
        }
    }

}
