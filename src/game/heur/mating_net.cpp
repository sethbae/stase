#include "heur.h"
#include "../king_net.hpp"
#include "../gamestate.hpp"

/**
 * @return true if the given colour can play a check in the current position, false otherwise.
 */
bool colour_can_play_check(const Gamestate & gs, const Colour c) {
    for (int i = 0; i < MAX_FRAMES; ++i) {
        const FeatureFrame & frame = gs.frames[check_hook.id][i];
        if (is_sentinel(frame.centre)) {
            return false;
        }
        if (colour(gs.board.get(frame.centre)) == c) {
            return true;
        }
    }
    return false;
}

/**
 * @return a fraction between zero and one, where 1 represents a king unable to move, and 0 represents a king with
 *  many flight squares.
 */
float score(const KingNet * king_net) {
    return ((float) (8 - king_net->flight_squares())) / 8.0f;
}

float __metrics::__mating_net(const Gamestate & gs) {

    bool white_attacks = gs.board.get_white();
    bool checks_exist =
        white_attacks
            ? colour_can_play_check(gs, WHITE)
            : colour_can_play_check(gs, BLACK);

    if (!checks_exist) {
        return 0.0f;
    }

    return white_attacks
        ? 1 * score(gs.b_king_net)
        : -1 * score(gs.w_king_net);
}
