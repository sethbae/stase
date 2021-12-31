#include "cands.h"

/**
 * Finds checks in the given gamestate. Adds feature frames to the given vector to record the ones it does find.
 * Finds checks playable by either colour, regardless of whose turn it is.
 * The format of the FeatureFrames is this:
 * -centre: the current square of a piece which can play a check
 * -secondary: the square on which it can play a check
 * -conf1: unused
 * -conf2: unused
 */
void find_checks_hook(Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

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

        if (moving_king && !would_be_safe_for_king_after(gs, m.to, m, colour(gs.board.get(m.from)))) {
            continue;
        }

        if (!would_be_safe_for_king_after(gs, k_sq, m, colour(gs.board.get(k_sq)))) {
            frames.push_back(
              FeatureFrame{s, m.to, 0, 0 }
            );
        }
    }

}

void play_check(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {
    if (counter.has_space() && !gs.is_kpinned_piece(ff->centre, get_delta_between(ff->centre, ff->secondary))) {

        Move m{ff->centre, ff->secondary, 0};
        int score;
        if (would_be_weak_after(gs, m.to, colour(gs.board.get(m.from)), m)) {
            score = unsafe_check_score(gs.board.get(m.from));
        } else {
            score = safe_check_score(gs.board.get(m.from));
        }
        m.set_score(score);

        moves[counter.inc()] = m;
    }
}
