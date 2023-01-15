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
float flight_squares_score(const KingNet * king_net) {
    return ((float) (8 - king_net->flight_squares())) / 8.0f;
}

float back_rank_mate_score(const KingNet * king_net) {

    Square k_sq = king_net->king_square();
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
        return 0.0f;
    }

    if ((val(luft1) && king_net->is_flight_square(luft1))
        || (val(luft2) && king_net->is_flight_square(luft2))
        || (val(luft3) && king_net->is_flight_square(luft3))) {
        return 0.0f;
    }
    return 1.0f;
}

float rank_file_score(const Square s) {
    static const float scores[8][8] = {
        {3.0f, 2.5f, 2.2f, 2.0f, 2.0f, 2.2f, 2.5f, 3.0f},
        {2.5f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.5f},
        {2.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.2f},
        {2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f},
        {2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f},
        {2.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.2f},
        {2.5f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.5f},
        {3.0f, 2.5f, 2.2f, 2.0f, 2.0f, 2.2f, 2.5f, 3.0f}
    };
    return scores[s.x][s.y] / 3.0f;
}

float black_score(const Gamestate & gs) {
    return flight_squares_score(gs.b_king_net) + rank_file_score(gs.b_king) + back_rank_mate_score(gs.b_king_net);
}

float white_score(const Gamestate & gs) {
    return flight_squares_score(gs.w_king_net) + rank_file_score(gs.w_king) + back_rank_mate_score(gs.w_king_net);
}

float __metrics::__mating_net(const Gamestate & gs) {

    if (!colour_can_play_check(gs, WHITE) && !colour_can_play_check(gs, BLACK) && !gs.in_check) {
        return 0.0f;
    }

    return 0.5f * (black_score(gs) - white_score(gs));
}
