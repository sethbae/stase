#include "cands.h"
#include "hook.hpp"
#include "../gamestate.hpp"

/**
 * Writes feature frames for pieces which could feature in a positional move, defined as anything other than a pawn or
 * king. Format of the FeatureFrame:
 * centre: the square of the piece
 * secondary: sentinel
 * conf1: unused
 * conf2: unused
 */
bool positional_move_hook(Gamestate & gs, const Square s) {
    if (is_not_pk(gs.board.get(s)) && !gs.is_kpinned_piece(s)) {
        return gs.add_frame(positional_hook.id, FeatureFrame{s, SQUARE_SENTINEL, 0, 0});
    }
    return true;
}

/**
 * Writes feature frames for each king, so that positional endgame moves can be played. Format of the FeatureFrame:
 * centre: the square of the king
 * secondary: sentinel
 * conf1: unused
 * conf2: unused
 */
bool king_positional_hook(Gamestate & gs, const Square s) {
    if (type(gs.board.get(s)) == KING) {
        return gs.add_frame(king_position_hook.id, FeatureFrame{s, SQUARE_SENTINEL, 0, 0});
    }
    return true;
}

const Hook positional_hook{
    "position",
    9,
    &positional_move_hook
};

const Hook king_position_hook{
    "king-position",
    11,
    &king_positional_hook
};
