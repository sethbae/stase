#include "game.h"
#include "cands.h"
#include "../gamestate.hpp"

/**
 * Wraps the weak square logic in a hook which, if the square is weak, records a feature frame
 * including the value of the weakest attackers (min_w in conf_1, min_b in conf_2).
 * This hook only records weak squares which have unsafe pieces on them.
 */
bool is_unsafe_piece_hook(Gamestate & gs, const Square s) {

    Piece p = gs.board.get(s);
    Colour c = colour(p);
    if (p == EMPTY) { return true; }

    SquareControlStatus ss = gs.control_cache->get_control_status(s);

    if (is_weak_status(gs, s, c, ss)) {
        return gs.add_frame(unsafe_piece_hook.id, FeatureFrame{s, SQUARE_SENTINEL, ss.min_w, ss.min_b});
    }

    if (pawn_is_en_passantable(gs, s)) {
        return gs.add_frame(unsafe_piece_hook.id, FeatureFrame{s, SQUARE_SENTINEL, ss.min_w, ss.min_b});
    }

    return true;
}

const Hook unsafe_piece_hook {
    "unsafe-piece",
    0,
    &is_unsafe_piece_hook,
};
