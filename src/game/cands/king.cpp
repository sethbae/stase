#include "responder.hpp"
#include "../gamestate.hpp"

int approach_kings(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {

    const Square k_sq = ff->centre;
    const Colour king_colour = colour(gs.board.get(k_sq));
    const Square enemy_k_sq = king_colour == WHITE ? gs.b_king : gs.w_king;

    int signum_x = (enemy_k_sq.x > k_sq.x) ? 1 : -1;
    int signum_y = (enemy_k_sq.y > k_sq.y) ? 1 : -1;

    Square dest = mksq(k_sq.x + signum_x, k_sq.y + signum_y);

    if (would_be_safe_king_square(gs, dest, king_colour)) {
        if (idx < end) {
            Move m{k_sq, dest, 0};
            m.set_score(approach_kings_score());
            moves[idx++] = m;
        }
    }
    return idx;
}

const Responder approach_kings_resp{
    "approach-kings",
    &approach_kings
};
