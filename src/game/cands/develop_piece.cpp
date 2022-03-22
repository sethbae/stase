#include "cands.h"
#include "../gamestate.hpp"
#include "responder.hpp"

const int BISHOPS[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 2, 0, 2, 2, 0, 2, 0,
        1, 0, 0, 1, 1, 0, 0, 1,
        0, 2, 1, 0, 0, 1, 2, 0,
        0, 2, 1, 0, 0, 1, 2, 0,
        1, 0, 0, 1, 1, 0, 0, 1,
        0, 2, 0, 2, 2, 0, 2, 0,
        0, 0, 0, 0, 0, 0, 0, 0
};

const int KNIGHTS[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
};

const int ROOKS[64] = {
        0, 0, 2, 2, 2, 2, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 2, 2, 2, 2, 0, 0
};

const int QUEENS[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 2, 2, 2, 0, 0, 0,
        0, 1, 0, 0, 0, 1, 0, 0,
        1, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 1, 1,
        0, 1, 0, 0, 0, 1, 0, 0,
        0, 0, 2, 2, 2, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
};

const Ptype BACK_RANK[] = {
        ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
};

const int XKN_LEGAL_W[] = {1, 2, -1, -2};
const int YKN_LEGAL_W[] = {2, 1, 2, 1};

const int XKN_LEGAL_B[] = {1, 2, -1, -2};
const int YKN_LEGAL_B[] = {-2, -1, -2, -1};

Square best_square(const Gamestate & gs, Square s, const int xd, const int yd, const int * scores) {

    Square best_sq;
    int best_val = 0;

    int x = get_x(s) + xd, y = get_y(s) + yd;
    Square temp;

    if (gs.is_kpinned_piece(s, Delta{(SignedByte) xd, (SignedByte) yd})) {
        return SQUARE_SENTINEL;
    }

    while (val(temp = mksq(x, y))) {

        Piece p = gs.board.get(temp);

        if (p == EMPTY) {
            if (scores[y*8 + x] > best_val) {
                // empty square
                best_sq = temp;
                best_val = scores[y * 8 + x];
            }
        } else if (colour(p) != colour(gs.board.get(s))) {
            // piece which can be captured
            if (scores[y*8 + x] > best_val) {
                best_sq = temp;
                best_val = scores[y*8 + x];
            }
            break;
        } else {
            // blocking square
            break;
        }

        x += xd;
        y += yd;
    }

    if (best_val > 0) {
        return best_sq;
    } else {
        return SQUARE_SENTINEL;
    }

}

int best_diag_squares(const Gamestate & gs, Square s, Move * moves, int idx, int end, const int * scores) {

    int yd = gs.board.get_white() ? 1 : -1;

    // negative diagonal
    Square sq = best_square(gs, s, -1, yd, scores);
    if (!is_sentinel(sq) && idx < end) {
        moves[idx++] = Move{s, sq, 0};
    }

    // positive diagonal
    sq = best_square(gs, s, 1, yd, scores);
    if (!is_sentinel(sq) && idx < end) {
        moves[idx++] = Move{s, sq, 0};
    }

    return idx;
}

int best_ortho_squares(const Gamestate & gs, Square s, Move * moves, int idx, int end, const int * scores) {

    int yd = gs.board.get_white() ? 1 : -1;

    // vertical
    Square sq = best_square(gs, s, 0, yd, scores);
    if (!is_sentinel(sq) && idx < end) {
        moves[idx++] = Move{s, sq, 0};
    }

    // left
    sq = best_square(gs, s, -1, 0, scores);
    if (!is_sentinel(sq) && idx < end) {
        moves[idx++] = Move{s, sq, 0};
    }

    // right
    sq = best_square(gs, s, 1, 0, scores);
    if (!is_sentinel(sq) && idx < end) {
        moves[idx++] = Move{s, sq, 0};
    }

    return idx;
}

int best_knight_square(const Gamestate & gs, Square s, Move * moves, int idx, int end) {

    const bool white = (colour(gs.board.get(s)) == WHITE);
    const int * xd = white ? XKN_LEGAL_W : XKN_LEGAL_B;
    const int * yd = white ? YKN_LEGAL_W : YKN_LEGAL_B;

    Square best_sq;
    int best_val = 0;
    
    if (gs.is_kpinned_piece(s, KNIGHT_DELTA)) {
        return idx;
    }

    for (int i = 0; i < 4; ++i) {
        Square temp = mksq(get_x(s) + xd[i], get_y(s) + yd[i]);
        int score = KNIGHTS[get_y(temp)*8 + get_x(temp)];
        if (val(temp) && score > best_val && colour(gs.board.get(temp)) != colour(gs.board.get(s))) {
            best_sq = temp;
            best_val = score;
        }
    }

    if (best_val > 0 && idx < end) {
        moves[idx++] = Move{s, best_sq, 0};
    }

    return idx;

}

/**
 * Checks whether or not the side to move can castle by performing checks on the given squares.
 * @param ksq1 the first square the king must pass through or land on
 * @param ksq2 the second square the king must pass through or land on
 * @param long_castle_square if castling long, the final square which must be empty. If castling short,
 * pass in SQUARE_SENTINEL here.
 */
bool check_squares_for_castling(
        const Gamestate & gs, const Square ksq1, const Square ksq2, const Square long_castle_square) {

    if (gs.board.get(ksq1) != EMPTY || gs.board.get(ksq2) != EMPTY
            || (!is_sentinel(long_castle_square) && gs.board.get(long_castle_square) != EMPTY)) {
        return false;
    }

    return would_be_safe_king_square(gs, ksq1, gs.board.get_white() ? WHITE : BLACK)
            && would_be_safe_king_square(gs, ksq2, gs.board.get_white() ? WHITE : BLACK);
}

/**
 * Tries to castle. Returns true if at least one move is suggested.
 */
int castling_moves(const Gamestate & gs, const Square s, Move * moves, int idx, int end) {

    if (gs.board.get_white() && equal(s, stosq("e1"))) {
        if (gs.board.get_cas_ws() && check_squares_for_castling(gs, stosq("f1"), stosq("g1"), SQUARE_SENTINEL)) {
            if (idx < end) {
                Move m{stosq("e1"), stosq("g1"), 0};
                m.set_cas();
                m.set_cas_short();
                moves[idx++] = m;
            } else {
                return idx;
            }
            // todo: early exits here?
        }
        if (gs.board.get_cas_wl() && check_squares_for_castling(gs, stosq("d1"), stosq("c1"), stosq("b1"))) {
            if (idx < end) {
                Move m{stosq("e1"), stosq("c1"), 0};
                m.set_cas();
                m.unset_cas_short();
                moves[idx++] = m;
            } else {
                return idx;
            }
        }
    } else if (!gs.board.get_white() && equal(s, stosq("e8"))) {
        if (gs.board.get_cas_bs() && check_squares_for_castling(gs, stosq("f8"), stosq("g8"), SQUARE_SENTINEL)) {
            if (idx < end) {
                Move m{stosq("e8"), stosq("g8"), 0};
                m.set_cas();
                m.set_cas_short();
                moves[idx++] = m;
            } else {
                return idx;
            }
        }
        if (gs.board.get_cas_bl() && check_squares_for_castling(gs, stosq("d8"), stosq("c8"), stosq("b8"))) {
            if (idx < end) {
                Move m{stosq("e8"), stosq("c8"), 0};
                m.set_cas();
                m.unset_cas_short();
                moves[idx++] = m;
            } else {
                return idx;
            }
        }
    }

    return idx;
}

/**
 * Suggests plausible pawn moves. Moves forward to supported (not necessarily safe) squares
 * including double moves.
 */
int pawn_moves(const Gamestate & gs, const Square sq, Move * moves, int idx, int end) {

    /*
     * Currently recommended moves are:
     *   a-h: single legal moves to pawn-defended squares
     *   c-f: double legal moves
     */

    int
        x = get_x(sq),
        y = get_y(sq);
    Square temp;
    const int FORWARD = gs.board.get_white() ? 1 : -1;

    // check one square in front; if it would be supported, move there
    if (gs.board.get(temp = mksq(x, y + FORWARD)) == EMPTY) {
        if ((gs.board.get_white() && w_pawn_defence_count(gs, temp) > 0)
                || (!gs.board.get_white() && b_pawn_defence_count(gs, temp) > 0)) {
            // check it's not pinned
            if (!gs.is_kpinned_piece(sq, get_delta_between(sq, temp))) {
                if (idx < end) {
                    moves[idx++] = Move{sq, temp, 0};
                } else {
                    // no more space, all done
                    return idx;
                }
            }
        }
    } else {
        // square in front is not empty, no moves
        return idx;
    }

    // don't double move on the wings
    if (x < 2 || x > 5) { return idx; }

    bool first_rank = gs.board.get_white() ? (y == 1) : (y == 6);
    if (first_rank && gs.board.get(temp = mksq(x, y + FORWARD + FORWARD)) == EMPTY) {
        if (!gs.is_kpinned_piece(sq, get_delta_between(sq, temp))) {
            if (idx < end) {
                moves[idx++] = Move{sq, temp, 0};
            } else {
                return idx;
            }
        }
    }
    return idx;
}

/**
 * Looks for the best square to develop the given piece to. Although this is deterministic
 * up to the position of the pieces, it may return more than one square for all pieces which
 * are not knights.
 */
int develop_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {

    bool is_white_piece = colour(gs.board.get(ff->centre)) == WHITE;
    if (gs.board.get_white() != is_white_piece) {
        return idx;
    }

    int new_idx;
    switch (type(gs.board.get(ff->centre))) {
        case BISHOP:
            return best_diag_squares(gs, ff->centre, moves, idx, end, BISHOPS);
        case KNIGHT:
            return best_knight_square(gs, ff->centre, moves, idx, end);
        case ROOK:
            return best_ortho_squares(gs, ff->centre, moves, idx, end, ROOKS);
        case QUEEN:
            new_idx = best_diag_squares(gs, ff->centre, moves, idx, end, QUEENS);
            if (new_idx < end) {
                return best_ortho_squares(gs, ff->centre, moves, new_idx, end, QUEENS);
            } else {
                return new_idx;
            }
        case KING:
            return castling_moves(gs, ff->centre, moves, idx, end);
        case PAWN:
            return pawn_moves(gs, ff->centre, moves, idx, end);
        default:
            return idx;
    }

}

/**
 * Checks if the given square contains an undeveloped piece.
 * @param ff the feature frame to record details in, if so.
 */
bool is_undeveloped_piece(const Gamestate & gs, Square centre) {
    if (get_y(centre) == 0 || get_y(centre) == 7) {
        return type(gs.board.get(centre)) == BACK_RANK[get_x(centre)];
    } else if (get_y(centre) == 1 && gs.board.get(centre) == W_PAWN) {
        return true;
    } else if (get_y(centre) == 6 && gs.board.get(centre) == B_PAWN) {
        return true;
    } else {
        return false;
    }
}

/**
 * Wraps the is_undeveloped_piece method in a hook which, if there is an undeveloped piece,
 * pushes a new FeatureFrame onto the list.
 */
bool is_undeveloped_piece_hook(Gamestate & gs, const Square centre) {
    if (is_undeveloped_piece(gs, centre)) {
        return gs.add_frame(develop_hook.id, FeatureFrame{centre, SQUARE_SENTINEL, 0, 0});
    }
    return true;
}

const Hook develop_hook {
    "undeveloped-piece",
    1,
    &is_undeveloped_piece_hook
};

const Responder develop_resp{
    "develop",
    &develop_piece
};
