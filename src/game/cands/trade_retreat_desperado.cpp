#include "cands.h"
#include "../gamestate.hpp"
#include "responder.hpp"

/**
 * Tries to trade off the unsafe piece with any piece of greater or equal value. Suggests all
 * pieces sharing the maximum value available.
 */
void trade_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {

    std::vector<Move> piece_moves;
    piece_moves.reserve(32);

    piecemoves_ignore_check(gs.board, ff->centre, piece_moves);

    // find the maximum value available
    int max_val = 0;
    for (int i = 0; i < piece_moves.size(); ++i) {
        Piece p = gs.board.get(piece_moves[i].to);
        if (p != EMPTY && piece_value(p) > max_val
                && !gs.is_kpinned_piece(ff->centre, get_delta_between(piece_moves[i].from, piece_moves[i].to))) {
            max_val = piece_value(p);
        }
    }

    if (max_val < piece_value(gs.board.get(ff->centre))) {
        return;
    }

    // return a trade with all pieces of this maximum value
    for (int i = 0; i < piece_moves.size(); ++i) {
        if (piece_value(gs.board.get(piece_moves[i].to)) == max_val
                && !gs.is_kpinned_piece(ff->centre, get_delta_between(piece_moves[i].from, piece_moves[i].to))) {
            if (counter.has_space()) {
                Piece trade_this = gs.board.get(ff->centre);
                Piece for_this = gs.board.get(piece_moves[i].to);
                piece_moves[i].set_score(trade_score(trade_this, for_this));
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

    std::vector<Move> piece_moves;
    piece_moves.reserve(32);

    piecemoves_ignore_check(gs.board, ff->centre, piece_moves);

    bool is_king = type(gs.board.get(ff->centre)) == KING;

    for (int i = 0; i < piece_moves.size(); ++i) {

        bool would_be_unsafe;

        if (is_king) {
            const Colour c = colour(gs.board.get(ff->centre));
            would_be_unsafe = !would_be_safe_for_king_after(gs, piece_moves[i].to, piece_moves[i], c);
        } else {
            would_be_unsafe = weak_piece(gs, piece_moves[i].to, piece_moves[i]);
        }

        if (!would_be_unsafe && !gs.is_kpinned_piece(ff->centre, get_delta_between(piece_moves[i].from, piece_moves[i].to))) {
            if (counter.has_space()) {
                piece_moves[i].set_score(retreat_score(gs.board.get(piece_moves[i].from)));
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

    std::vector<Move> piece_moves;
    piece_moves.reserve(32);

    piecemoves_ignore_check(gs.board, ff->centre, piece_moves);

    // find the maximum value available
    int max_val = 0;
    Move max_move = MOVE_SENTINEL;

    for (int i = 0; i < piece_moves.size(); ++i) {
        Piece p = gs.board.get(piece_moves[i].to);
        if (p != EMPTY && piece_value(p) > max_val
                && !gs.is_kpinned_piece(ff->centre, get_delta_between(piece_moves[i].from, piece_moves[i].to))) {
            max_val = piece_value(p);
            max_move = piece_moves[i];
        }
    }

    // return a trade with the piece of maximum value
    if (counter.has_space() && !is_sentinel(max_move)) {
        max_move.set_score(desperado_score(max_val));
        moves[counter.inc()] = max_move;
    } else {
        return;
    }

}

const Responder trade_resp{
    "trade",
    &trade_piece
};

const Responder retreat_resp{
    "retreat",
    &retreat_piece
};

const Responder desperado_resp{
    "desperado",
    &desperado_piece
};
