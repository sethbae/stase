#include "cands/cands.h"
#include "gamestate.hpp"

const float UNSAFE_WEIGHT = 1.0f;
const float FORK_WEIGHT = 1.0f;
const float PROMOTION_WEIGHT = 1.0f;

inline int count_frames(const Gamestate & gs, int hook_id) {
    int sum = 0;
    for (int i = 0; i < MAX_FRAMES && !is_sentinel(gs.frames[hook_id][i].centre); ++i) {
        ++sum;
    }
    return sum;
}

inline int count_frames(const Gamestate & gs, int hook_id, Colour c) {
    int sum = 0;
    for (int i = 0; i < MAX_FRAMES && !is_sentinel(gs.frames[hook_id][i].centre); ++i) {
        if (colour(gs.board.get(gs.frames[hook_id][i].centre)) == c) {
            ++sum;
        }
    }
    return sum;
}

/**
 * Returns a score representing how quiescent a position is. The score will be non-negative,
 * and the higher it is, the crazier the position is.
 */
float quiess(Gamestate & gs) {

    if (gs.in_check) {
        discover_feature_frames(gs, unsafe_piece_hook);
    }

    Colour colour_to_move = gs.board.get_white() ? WHITE : BLACK;

    int unsafe = 0,
        forks = count_frames(gs, fork_hook.id, colour_to_move),
        proms = count_frames(gs, promotion_hook.id, colour_to_move);

    for (int i = 0; i < MAX_FRAMES; ++i) {
        const FeatureFrame & ff = gs.frames[unsafe_piece_hook.id][i];
        if (is_sentinel(ff.centre)) {
            break;
        }
        if (is_major_piece(gs.board.get(ff.centre))) {
            return piece_value(gs.board.get(ff.centre));
        }
        ++unsafe;
    }

    return (gs.in_check ? 1 : 0)
            + (unsafe * UNSAFE_WEIGHT)
            + (forks * FORK_WEIGHT)
            + (proms * PROMOTION_WEIGHT);

}
