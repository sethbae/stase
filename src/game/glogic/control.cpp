#include "glogic.h"
#include "../gamestate.hpp"

/**
 * Checks whether a piece of the given type, standing on the from square, could move
 * onto the to square. This allows for captures, and does not consider whether the piece
 * is pinned or not. This assumes that the piece is able to move in the direction leading
 * from from to to.
 */
bool can_see_immediately(const Gamestate & gs, const Piece p, const Square from, const Square to) {

    Delta d = get_delta_between(from, to);
    Square fpe = gs.first_piece_encountered(from, d);

    if (is_sentinel(fpe)) { return true; }
    if (d.dx == 0) {
        // y is increasing, so we can reach any square on the line with y less than the fpe's, and vice versa
        if (d.dy > 0 && to.y < fpe.y) { return true; }
        if (d.dy < 0 && to.y > fpe.y) { return true; }
        if (to.y == fpe.y) { return colour(gs.board.get(fpe)) != colour(p); }
    } else {
        // x is increasing, so we can reach any square on the line with x less than the fpe's, and vice versa
        if (d.dx > 0 && to.x < fpe.x) { return true; }
        if (d.dx < 0 && to.x > fpe.x) { return true; }
        if (to.x == fpe.x) { return colour(gs.board.get(fpe)) != colour(p); }
    }
    return false;
}

/**
 * Checks whether a piece of the given type, if it were standing on the from square,
 * would be able to control the to square, permitting x-rays. This does not take
 * account of pins, and assumes the piece given can indeed move in the direction required.
 */
bool can_see_x_ray(const Gamestate & gs, const Piece p, const Square from, const Square to) {

    Delta d = get_delta_between(from, to);
    bool x_changing = (d.dx != 0);
    bool already_x_raying = false;
    Square fpe = gs.first_piece_encountered(from, d);

    while (!is_sentinel(fpe) && ((x_changing && fpe.x < to.x) || (!x_changing && fpe.y < to.y))) {
        Piece other_p =  gs.board.get(fpe);
        if (colour(other_p) != colour(p)) {
            if (already_x_raying) {
                // if we hit an enemy piece and we were already in an x-ray, we can't see the square
                return false;
            } else {
                // otherwise, however, we can continue (now in an x-ray)
                already_x_raying = true;
                fpe = gs.first_piece_encountered(fpe, d);
                continue;
            }
        }
        if (can_move_in_direction(other_p, d)) {
            // x-rayable piece: loop
            fpe = gs.first_piece_encountered(fpe, d);
        } else {
            // not x-rayable, cannot see square
            return false;
        }
    }

    // sentinel encountered or square beyond(at) the target square
    return true;
}

/**
 * Returns true if the square contains a piece which is not favourably controlled
 * for its colour (ie non-negative for black, non-positive for white).
 */
bool zero_or_worse_control(const Gamestate & gs, const Square s) {
    SquareControlStatus status = gs.control_cache->get_control_status(s);
    if (colour(gs.board.get(s)) == WHITE) {
        return status.balance <= 0 && status.min_w > piece_value(PAWN);
    } else {
        return status.balance >= 0 && status.min_b > piece_value(PAWN);
    }
}

/**
 * Returns true iff the given colour does not defend the given square whatsoever.
 * A defender is counted even if it is pinned.
 */
bool totally_undefended(const Gamestate & gs, const Colour c, const Square s) {
    SquareControlStatus status = gs.control_cache->get_control_status(s);
    return c == WHITE
        ? status.min_w == NOT_ATTACKED_AT_ALL
        : status.min_b == NOT_ATTACKED_AT_ALL;
}
