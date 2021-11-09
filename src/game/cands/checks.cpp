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
void find_checks_hook(const Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    // find the enemy king
    Square k_sq;
    Piece enemy_king = colour(gs.board.get(s)) == WHITE ? B_KING : W_KING;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(mksq(x, y)) == enemy_king) {
                k_sq = mksq(x, y);
            }
        }
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

        if (!would_be_safe_for_king_after(gs, k_sq, m, colour(enemy_king))) {
            frames.push_back(
              FeatureFrame{s, m.to, 0, 0 }
            );
        }
    }

}

void play_check(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {
    if (counter.has_space()) {
        moves[counter.inc()] = Move{ff->centre, ff->secondary, 0};
    }
}
