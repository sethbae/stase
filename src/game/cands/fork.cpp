#include "game.h"
#include "cands.h"
#include "../heur/heur.h"

void find_knight_forks(const Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    for (int j = 0; j < 8; ++j) {

        Square fork_square = mksq(get_x(s) + XKN[j], get_y(s) + YKN[j]);

        if (!val(fork_square)
            || colour(gs.board.get(fork_square)) == colour(gs.board.get(s))
            || would_be_unsafe_piece_after(gs, fork_square, Move{s, fork_square})) {
            continue;
        }

        int forked_count = 0;

        for (int k = 0; k < 8; ++k) {

            Square forked_square = mksq(get_x(fork_square) + XKN[k], get_y(fork_square) + YKN[k]);

            if (!val(forked_square)
                || forked_square == s) {
                continue;
            }

            Piece p = gs.board.get(forked_square);

            if (colour(p) != colour(gs.board.get(s))
                && type(p) != KNIGHT
                && (would_be_unsafe_piece_after(gs, forked_square, Move{s, fork_square})
                    || piece_value(p) > piece_value(KNIGHT))) {
                ++forked_count;
            }
        }

        if (forked_count >= 2) {
            frames.push_back(FeatureFrame{s, fork_square, 0, 0});
        }
    }

}

int count_forkable_pieces_after(const Gamestate & gs, const Move move, MoveType dir) {

    int idx_start, idx_stop;
    int count = 0;

    if (dir == ORTHO) {
        idx_start = ORTHO_START;
        idx_stop = ORTHO_STOP;
    } else {
        idx_start = DIAG_START;
        idx_stop = DIAG_STOP;
    }

    for (int i = idx_start; i < idx_stop; ++i) {

        Square first_piece_sq = first_piece_encountered(gs.board, move.to, DeltaPair{(Byte) XD[i], (Byte) YD[i]});
        Piece first_p = gs.board.get(first_piece_sq);

        // check that some piece was actually encountered
        if (first_piece_sq == SQUARE_SENTINEL) { continue; }
        // of the opposite colour
        if (colour(first_p) == colour(gs.board.get(move.from))) { continue; }

        // only fork weak pieces or more valuable ones
        if (piece_value(first_p) > piece_value(gs.board.get(move.from))
                || would_be_unsafe_piece_after(gs, first_piece_sq, move)) {
            ++count;
        }

    }

    return count;

}

void find_forks(
        const Gamestate & gs,
        const Square forker_sq,
        const bool ortho,
        const bool diag,
        std::vector<FeatureFrame> & frames) {

    Square temp;

    // fish out the correct indices to get the deltas
    int start_idx, end_idx;
    if (ortho && diag) { start_idx = DIAG_START, end_idx = ORTHO_STOP; }
    else if (ortho) { start_idx = ORTHO_START, end_idx = ORTHO_STOP; }
    else if (diag) { start_idx = DIAG_START, end_idx = DIAG_STOP; }
    else { return; }

    // for each sliding direction
    for (int j = start_idx; j < end_idx; ++j) {

        bool cont = true;
        int x = get_x(forker_sq) + XD[j], y = get_y(forker_sq) + YD[j];

        // perform a 'beta' walk, ie empty squares and at most one capture-square
        while (val(temp = mksq(x, y)) && cont) {

            // check for forks:
            // - there isn't a piece on the square of the queen's colour (illegal move)
            // - the square is safe
            if (colour(gs.board.get(temp)) != colour(gs.board.get(forker_sq))
                    && !would_be_unsafe_piece_after(gs, temp, Move{forker_sq, temp})) {

                int forked_pieces = 0;

                if (ortho) {
                    forked_pieces += count_forkable_pieces_after(gs, Move{forker_sq, temp}, ORTHO);
                }
                if (diag) {
                    forked_pieces += count_forkable_pieces_after(gs, Move{forker_sq, temp}, DIAG);
                }

                if (forked_pieces >= 2) {
                    frames.push_back(FeatureFrame{forker_sq, temp, 0, 0});
                }

            }

            // loop logic: stop if the square had a piece on it, otherwise take another step
            if (gs.board.get(temp) != EMPTY) {
                cont = false;
            } else {
                x += XD[j];
                y += YD[j];
            }
        }
    }

}

void find_forks_hook(const Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    switch (type(gs.board.get(s))) {
        case KNIGHT:
            find_knight_forks(gs, s, frames); return;
        case BISHOP:
            find_forks(gs, s, false, true, frames); return;
        case ROOK:
            find_forks(gs, s, true, false, frames); return;
        case QUEEN:
            find_forks(gs, s, true, true, frames); return;
        default:
            return;
    }

}
