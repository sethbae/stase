#include "../../../include/stase/game.h"
#include "glogic.h"
#include "../gamestate.hpp"

/**
 * Checks whether in the current position, a king would be safe on the given square.
 * This is evaluated as if the king's next move were from its current location to the
 * square given, although note that there is no requirement for this move to be legal.
 * @param colour: the colour of the king to check this for.
 */
bool would_be_safe_king_square(const Gamestate & gs, const Square s, const Colour colour) {
    SquareControlStatus status = gs.control_cache->get_control_status(s);
    return colour == WHITE
        ? status.min_b > ATTACKED_BY_PINNED_PIECE
        : status.min_w > ATTACKED_BY_PINNED_PIECE;
}

/**
 * Checks whether the given-coloured king is on a safe square or not. This is functionally equivalent
 * to the in_check functions in the board module but may be implemented more efficiently. The value of
 * the duplication is that the slow and reliable variation can be used to check the correctness of the
 * performance-critical version.
 */
bool has_safe_king(const Gamestate & gs, const Colour king_colour) {
    return would_be_safe_king_square(gs, king_colour == WHITE ? gs.w_king : gs.b_king, king_colour);
}

bool has_luft(const Gamestate & gs, const Colour c) {

    const KingNet * king_net = (c == WHITE ? gs.w_king_net : gs.b_king_net);
    const Square k_sq = king_net->king_square();
    Square luft1, luft2, luft3;

    if (k_sq.y == 7) {

        // king is on a file
        luft1 = mksq(k_sq.x - 1, k_sq.y - 1);
        luft2 = mksq(k_sq.x    , k_sq.y - 1);
        luft3 = mksq(k_sq.x + 1, k_sq.y - 1);

    } else if (k_sq.y == 0) {

        // king is on a file
        luft1 = mksq(k_sq.x - 1, k_sq.y + 1);
        luft2 = mksq(k_sq.x    , k_sq.y + 1);
        luft3 = mksq(k_sq.x + 1, k_sq.y + 1);

    } else if (k_sq.x == 7) {

        // king is on 8th rank
        luft1 = mksq(k_sq.x - 1, k_sq.y - 1);
        luft2 = mksq(k_sq.x - 1, k_sq.y);
        luft3 = mksq(k_sq.x - 1, k_sq.y + 1);

    } else if (k_sq.x == 0) {

        // king is on first rank
        luft1 = mksq(k_sq.x + 1, k_sq.y - 1);
        luft2 = mksq(k_sq.x + 1, k_sq.y);
        luft3 = mksq(k_sq.x + 1, k_sq.y + 1);

    } else {
        // king is not on edge of board
        return true;
    }

    if ((val(luft1) && king_net->is_flight_square(luft1))
        || (val(luft2) && king_net->is_flight_square(luft2))
        || (val(luft3) && king_net->is_flight_square(luft3))) {
        return true;
    }

    return false;
}
