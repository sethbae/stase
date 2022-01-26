#include "game.h"
#include "cands.h"
#include "../gamestate.hpp"
#include "../glogic/glogic.h"

void find_knight_forks(const Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    for (int j = 0; j < 8; ++j) {

        Square fork_square = mksq(get_x(s) + XKN[j], get_y(s) + YKN[j]);

        if (!val(fork_square)
            || colour(gs.board.get(fork_square)) == colour(gs.board.get(s))
            || would_be_unsafe_after(gs, fork_square, Move{s, fork_square, 0})) {
            continue;
        }

        int forked_count = 0;
        int val_a;
        int val_b;

        for (int k = 0; k < 8; ++k) {

            Square forked_square = mksq(get_x(fork_square) + XKN[k], get_y(fork_square) + YKN[k]);

            if (!val(forked_square)
                || equal(forked_square, s)) {
                continue;
            }

            Piece p = gs.board.get(forked_square);

            if (p != EMPTY
                && colour(p) != colour(gs.board.get(s))
                && type(p) != KNIGHT
                && (would_be_unsafe_after(gs, forked_square, Move{s, fork_square, 0})
                    || piece_value(p) > piece_value(KNIGHT))) {
                if (forked_count % 2 == 0) {
                    val_a = piece_value(p);
                } else {
                    val_b = piece_value(p);
                }
                ++forked_count;
            }
        }

        if (forked_count >= 2) {
            frames.push_back(FeatureFrame{s, fork_square, val_a, val_b});
        }
    }

}

/**
 * Returns true iff the given move poses a genuine, forking threat to the piece
 * on the given square. This is evaluated by looking at the two values, whether
 * the piece would become unsafe, whether it would be able to simply trade with
 * the forker etc.
 * Currently (GM-34), beta_covers doesn't evaluate knight moves, so this method
 * can't be used for knights.
 */
bool forkable(const Gamestate &gs, const Move m, const Square forked_piece_sq) {

    Piece forker_p = gs.board.get(m.from);
    Piece forked_p = gs.board.get(forked_piece_sq);

    // you can't fork an empty square!
    if (forked_p == EMPTY) {
        return false;
    }

    // don't fork pieces of the same colour
    if (colour(forked_p) == colour(forker_p)) {
        return false;
    }

    // don't fork pieces of the same type; these are always of the same value,
    // and can always move in the same direction as the forker
    if (type(forked_p) == type(forker_p)) {
        return false;
    }

    // the piece should either be more valuable, or should be made unsafe after the move
    if (piece_value(forked_p) > piece_value(gs.board.get(m.from))
        || would_be_unsafe_after(gs, forked_piece_sq, m)) {

        // avoid forking pieces which we already attacked
        if (!beta_covers(gs.board, m.from, forked_piece_sq)) {
            return true;
        }

    }

    return false;
}

/**
 * Checks the first piece encountered in each of four orthogonal/diagonal directions (according
 * to given [dir]). Counts the number of forkable pieces encountered, where a forkable piece is:
 * - of lower value than the piece moving (in the given [move])
 * - on an unsafe square after the [move]
 * This function will not consider movements
 */
inline int count_forkable_pieces_after(const Gamestate & gs, const Move move, MoveType dir, int & val_a, int & val_b) {

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

        Square forked_piece_square =
                first_piece_encountered(gs.board, move.to, Delta{(SignedByte) XD[i], (SignedByte) YD[i]});

        if (!is_sentinel(forked_piece_square)
                && forkable(gs, move, forked_piece_square)) {
            if (count % 2 == 0) {
                val_a = piece_value(gs.board.get(forked_piece_square));
            } else {
                val_b = piece_value(gs.board.get(forked_piece_square));
            }
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
                    && !would_be_unsafe_after(gs, temp, Move{forker_sq, temp, 0})) {

                int forked_pieces = 0;
                int val_a;
                int val_b;

                if (ortho) {
                    forked_pieces += count_forkable_pieces_after(gs, Move{forker_sq, temp, 0}, ORTHO, val_a, val_b);
                }
                if (diag) {
                    forked_pieces += count_forkable_pieces_after(gs, Move{forker_sq, temp, 0}, DIAG, val_a, val_b);
                }

                if (forked_pieces >= 2) {
                    frames.push_back(FeatureFrame{forker_sq, temp, val_a, val_b});
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

void find_pawn_forks(const Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    std::vector<Move> moves;
    moves.reserve(32);

    piecemoves_ignore_check(gs.board, s, moves);

    int yd = (colour(gs.board.get(s)) == WHITE) ? 1 : -1;

    for (int i = 0; i < moves.size(); ++i) {

        Move m = moves[i];

        if (!would_be_unsafe_after(gs, m.to, m)) {

            Square l_sq = mksq(get_x(m.to) - 1, get_y(m.to) + yd);
            Square r_sq = mksq(get_x(m.to) + 1, get_y(m.to) + yd);

            if (val(l_sq) && val(r_sq) && forkable(gs, m, l_sq) && forkable(gs, m, r_sq)) {
                frames.push_back(
                    FeatureFrame{
                    s,
                    m.to,
                    piece_value(gs.board.get(l_sq)),
                    piece_value(gs.board.get(r_sq))
                    }
                );
            }
        }
    }
}

void find_king_forks(const Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    Colour king_col = colour(gs.board.get(s));

    for (int i = 0; i < 8; ++i) {

        Square forker_sq = mksq(get_x(s) + XD[i], get_y(s) + YD[i]);

        if (val(forker_sq)
                && colour(gs.board.get(forker_sq)) != king_col
                && would_be_safe_king_square(gs, forker_sq, king_col)) {

            int count = 0;
            int val_a;
            int val_b;

            for (int j = 0; j < 8; ++j) {

                Square forked_sq = mksq(get_x(forker_sq) + XD[j], get_y(forker_sq) + YD[j]);

                // TODO (GM-34): until beta-covers correctly handles knights, pawns and kings,
                //  this check is necessary. If the total distance from the king's starting
                //  square is less than 1 in all directions, the piece was already attacked.
                int xd = get_x(forked_sq) - get_x(s), yd = get_y(forked_sq) - get_y(s);
                if ((-1 <= xd && xd <= 1) && (-1 <= yd && yd <= 1)) { continue; }

                if (val(forked_sq) && forkable(gs, Move{s, forker_sq, 0}, forked_sq)) {
                    if (count % 2 == 0) {
                        val_a = piece_value(gs.board.get(forked_sq));
                    } else {
                        val_b = piece_value(gs.board.get(forked_sq));
                    }
                    ++count;
                }

            }

            if (count >= 2) {
                frames.push_back(FeatureFrame{s, forker_sq, val_a, val_b});
            }
        }
    }

}

/**
 * Finds forks which can be played in the given gamestate. Adds feature frames to the given vector to
 * record the ones it does find. The format of the FeatureFrames is this:
 * -centre: the current square of a piece which can play a fork
 * -secondary: the square on which it can play a fork
 * -conf1: unused
 * -conf2: unused
 */
void find_forks_hook(Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    switch (type(gs.board.get(s))) {
        case KNIGHT:
            find_knight_forks(gs, s, frames); return;
        case BISHOP:
            find_forks(gs, s, false, true, frames); return;
        case ROOK:
            find_forks(gs, s, true, false, frames); return;
        case QUEEN:
            find_forks(gs, s, true, true, frames); return;
        case PAWN:
            find_pawn_forks(gs, s, frames); return;
        case KING:
            find_king_forks(gs, s, frames); return;
        default:
            return;
    }

}

/**
 * Responder which takes fork FeatureFrames and plays the fork described.
 */
void play_fork(const Gamestate & gs, const FeatureFrame * ff, Move * m, IndexCounter & counter) {
    if (counter.has_space() && !gs.is_kpinned_piece(ff->centre, get_delta_between(ff->centre, ff->secondary))) {
        Move move{ff->centre, ff->secondary, 0};
        move.set_score(fork_score(ff->conf_1, ff->conf_2));
        m[counter.inc()] = move;
    }
}
