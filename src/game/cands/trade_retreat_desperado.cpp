#include "cands.h"
#include "../gamestate.hpp"
#include "responder.hpp"

/**
 * Tries to trade off the unsafe piece with any piece of greater or equal value. Suggests all
 * pieces sharing the maximum value available.
 */
int trade_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {

    Move piece_moves_arr[32];
    ptr_vec<Move> piece_moves(piece_moves_arr, 32);

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
        return idx;
    }

    // return a trade with all pieces of this maximum value
    for (int i = 0; i < piece_moves.size(); ++i) {
        if (piece_value(gs.board.get(piece_moves[i].to)) == max_val
                && !gs.is_kpinned_piece(ff->centre, get_delta_between(piece_moves[i].from, piece_moves[i].to))) {
            if (idx < end) {
                Piece trade_this = gs.board.get(ff->centre);
                Piece for_this = gs.board.get(piece_moves[i].to);
                piece_moves[i].set_score(trade_score(trade_this, for_this));
                moves[idx++] = piece_moves[i];
            } else {
                return idx;
            }
        }
    }
    return idx;
}

/**
 * Tries to move the piece to a safe square. This square may be backwards or forwards, as long as
 * it is safe.
 */
int retreat_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {

    Move piece_moves_arr[32];
    ptr_vec<Move> piece_moves(piece_moves_arr, 32);

    piecemoves_ignore_check(gs.board, ff->centre, piece_moves);

    bool is_king = type(gs.board.get(ff->centre)) == KING;

    for (int i = 0; i < piece_moves.size(); ++i) {

        bool would_be_unsafe;

        if (is_king) {
            const Colour c = colour(gs.board.get(ff->centre));
            would_be_unsafe = !would_be_safe_king_square(gs, piece_moves[i].to, c);
        } else {
            would_be_unsafe = would_be_unsafe_after(gs, piece_moves[i].to, piece_moves[i]);
        }

        if (!would_be_unsafe && !gs.is_kpinned_piece(ff->centre, get_delta_between(piece_moves[i].from, piece_moves[i].to))) {
            if (idx < end) {
                piece_moves[i].set_score(retreat_score(gs.board.get(piece_moves[i].from)));
                moves[idx++] = piece_moves[i];
            } else {
                return idx;
            }
        }
    }
    return idx;
}

/**
 * Tries to capture absolutely any piece available. If none are available, does nothing.
 */
int desperado_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {

    Piece desperado = gs.board.get(ff->centre);
    Move piece_moves_arr[32];
    ptr_vec<Move> piece_moves(piece_moves_arr, 32);

    piecemoves_ignore_check(gs.board, ff->centre, piece_moves);

    // find the maximum value available
    int max_val = 0;
    Move max_move = MOVE_SENTINEL;

    for (int i = 0; i < piece_moves.size(); ++i) {
        Piece p = gs.board.get(piece_moves[i].to);
        if (p != EMPTY
            && piece_value(p) > max_val
            && !gs.is_kpinned_piece(ff->centre, get_delta_between(piece_moves[i].from, piece_moves[i].to))
            && (type(desperado) != KING || totally_undefended(gs, opposite_colour(desperado), piece_moves[i].to))) {
            max_val = piece_value(p);
            max_move = piece_moves[i];
        }
    }

    // return a trade with the piece of maximum value
    if (idx < end && !is_sentinel(max_move)) {
        max_move.set_score(desperado_score(max_val));
        moves[idx++] = max_move;
    }
    return idx;
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
