#include "cands.h"
#include "../../game/gamestate.hpp"
#include "responder.hpp"

/**
 * Finds checks in the given gamestate. Adds feature frames to the given vector to record the ones it does find.
 * Finds checks playable by either colour, regardless of whose turn it is.
 * The format of the FeatureFrames is this:
 * -centre: the current square of a piece which can play a check
 * -secondary: the square on which it can play a check
 * -conf1: unused
 * -conf2: unused
 */
bool find_checks_hook(Gamestate & gs, const Square s) {

    // find the enemy king
    Piece p = gs.board.get(s);
    Square k_sq;
    if (colour(p) == WHITE) {
        k_sq = gs.b_king;
    } else {
        k_sq = gs.w_king;
    }

    // find the squares this piece can move to
    bool moving_king = type(gs.board.get(s)) == KING;
    std::vector<Move> piece_moves;
    piece_moves.reserve(32);
    piecemoves_ignore_check(gs.board, s, piece_moves);

    // add a frame for each one which engenders an unsafe enemy king
    for (const Move & m : piece_moves) {

        if (moving_king && !would_be_safe_king_square(gs, m.to, colour(gs.board.get(m.from)))) {
            continue;
        }

        if (!would_be_safe_for_king_after(gs, k_sq, m, colour(gs.board.get(k_sq)))) {
            bool result = gs.add_frame(
                check_hook.id,
                FeatureFrame{s, m.to, 0, 0 }
            );
            if (!result) { return false; }
        }
    }
    return true;
}

int play_check(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {
    if (idx < end && !gs.is_kpinned_piece(ff->centre, get_delta_between(ff->centre, ff->secondary))) {

        Move m{ff->centre, ff->secondary, 0};
        int score;
        if (move_is_safe(gs, m)) {
            score = safe_check_score(gs.board.get(m.from));
        } else {
            score = unsafe_check_score(gs.board.get(m.from));
        }
        m.set_score(score);

        moves[idx++] = m;
    }
    return idx;
}

const Hook check_hook{
    "check",
    3,
    &find_checks_hook
};

const Responder play_check_resp{
    "check",
    &play_check
};
