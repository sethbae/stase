#include "cands.h"
#include "../heur/heur.h"

/**
 * Tries to trade off the unsafe piece with any piece of greater or equal value. Suggests all
 * pieces sharing the maximum value available.
 */
void trade_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {

    if (gs.is_kpinned_piece(ff->centre)) {
        return;
    }

    std::vector<Move> piece_moves;
    piece_moves.reserve(32);

    piecemoves_ignore_check(gs.board, ff->centre, piece_moves);

    // find the maximum value available
    int max_val = 0;
    for (int i = 0; i < piece_moves.size(); ++i) {
        Piece p = gs.board.get(piece_moves[i].to);
        if (p != EMPTY && piece_value(p) > max_val) {
            max_val = piece_value(p);
        }
    }

    if (max_val < piece_value(gs.board.get(ff->centre))) {
        return;
    }

    // return a trade with all pieces of this maximum value
    for (int i = 0; i < piece_moves.size(); ++i) {
        if (piece_value(gs.board.get(piece_moves[i].to)) == max_val) {
            if (counter.has_space()) {
                moves[counter.inc()] = piece_moves[i];
            } else {
                return;
            }
        }
    }

}

/**
 * Tries to move the piece to a safe square. This square may be backwards or forwards, as long as
 * it is safe.
 */
void retreat_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {

    if (gs.is_kpinned_piece(ff->centre)) {
        return;
    }

    std::vector<Move> piece_moves;
    piece_moves.reserve(32);

    piecemoves_ignore_check(gs.board, ff->centre, piece_moves);

    for (int i = 0; i < piece_moves.size(); ++i) {
        if (!would_be_unsafe_after(gs, piece_moves[i].to, piece_moves[i])) {
            if (counter.has_space()) {
                moves[counter.inc()] = piece_moves[i];
            } else {
                return;
            }
        }
    }
}

/**
 * Tries to capture absolutely any piece available. If none are available, does nothing.
 */
void desperado_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {

    if (gs.is_kpinned_piece(ff->centre)) {
        return;
    }

    std::vector<Move> piece_moves;
    piece_moves.reserve(32);

    piecemoves_ignore_check(gs.board, ff->centre, piece_moves);

    // find the maximum value available
    int max_val = 0;
    Move max_move = MOVE_SENTINEL;

    for (int i = 0; i < piece_moves.size(); ++i) {
        Piece p = gs.board.get(piece_moves[i].to);
        if (p != EMPTY && piece_value(p) > max_val) {
            max_val = piece_value(p);
            max_move = piece_moves[i];
        }
    }

    // return a trade with the piece of maximum value
    if (counter.has_space() && !is_sentinel(max_move)) {
        moves[counter.inc()] = max_move;
    } else {
        return;
    }

}
