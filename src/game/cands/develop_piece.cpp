#include "cands.h"

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

Square best_square(const Board & b, Square s, const int xd, const int yd, const int * scores) {

    Square best_sq;
    int best_val = 0;

    int x = get_x(s) + xd, y = get_y(s) + yd;
    Square temp;

    while (val(temp = mksq(x, y))) {

        Piece p = b.get(temp);

        if (colour(p) == EMPTY) {
            if (scores[y*8 + x] > best_val) {
                // empty square
                best_sq = temp;
                best_val = scores[y * 8 + x];
            }
        } else if (colour(p) != colour(b.get(s))) {
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

void best_diag_squares(const Board & b, Square s, Move * moves, IndexCounter & move_counter, const int * scores) {

    int yd = b.get_white() ? 1 : -1;

    // negative diagonal
    Square sq = best_square(b, s, -1, yd, scores);
    if (sq != SQUARE_SENTINEL && move_counter.has_space()) {
        moves[move_counter.inc()] = Move{s, sq, 0};
    }

    // positive diagonal
    sq = best_square(b, s, 1, yd, scores);
    if (sq != SQUARE_SENTINEL && move_counter.has_space()) {
        moves[move_counter.inc()] = Move{s, sq, 0};
    }

    return;
}

void best_ortho_squares(const Board & b, Square s, Move * moves, IndexCounter & move_counter, const int * scores) {

    int yd = b.get_white() ? 1 : -1;

    // vertical
    Square sq = best_square(b, s, 0, yd, scores);
    if (sq != SQUARE_SENTINEL && move_counter.has_space()) {
        moves[move_counter.inc()] = Move{s, sq, 0};
    }

    // left
    sq = best_square(b, s, -1, 0, scores);
    if (sq != SQUARE_SENTINEL && move_counter.has_space()) {
        moves[move_counter.inc()] = Move{s, sq, 0};
    }

    // right
    sq = best_square(b, s, 1, 0, scores);
    if (sq != SQUARE_SENTINEL && move_counter.has_space()) {
        moves[move_counter.inc()] = Move{s, sq, 0};
    }

    return;
}

void best_knight_square(const Board & b, Square s, Move * moves, IndexCounter & move_counter) {

    const bool white = (colour(b.get(s)) == WHITE);
    const int * xd = white ? XKN_LEGAL_W : XKN_LEGAL_B;
    const int * yd = white ? YKN_LEGAL_W : YKN_LEGAL_B;

    Square best_sq;
    int best_val = 0;

    for (int i = 0; i < 4; ++i) {
        Square temp = mksq(get_x(s) + xd[i], get_y(s) + yd[i]);
        int score = KNIGHTS[get_y(temp)*8 + get_x(temp)];
        if (val(temp) && score > best_val && colour(b.get(temp)) != colour(b.get(s))) {
            best_sq = temp;
            best_val = score;
        }
    }

    if (best_val > 0 && move_counter.has_space()) {
        moves[move_counter.inc()] = Move{s, best_sq, 0};
    }

    return;

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
            || (long_castle_square != SQUARE_SENTINEL && gs.board.get(long_castle_square) != EMPTY)) {
        return false;
    }

    return would_be_safe_king_square(gs, ksq1, gs.board.get_white() ? WHITE : BLACK)
            && would_be_safe_king_square(gs, ksq2, gs.board.get_white() ? WHITE : BLACK);
}

/**
 * Tries to castle. Returns true if at least one move is suggested.
 */
bool castling_moves(const Gamestate & gs, const Square s, Move * moves, IndexCounter & counter) {

    bool found = false;

    if (gs.board.get_white() && s == stosq("e1")) {
        if (gs.board.get_cas_ws() && check_squares_for_castling(gs, stosq("f1"), stosq("g1"), SQUARE_SENTINEL)) {
            if (counter.has_space()) {
                moves[counter.inc()] = Move{stosq("e1"), stosq("g1"), 0};
                found = true;
            }
        }
        if (gs.board.get_cas_wl() && check_squares_for_castling(gs, stosq("d1"), stosq("c1"), stosq("b1"))) {
            if (counter.has_space()) {
                moves[counter.inc()] = Move{stosq("e1"), stosq("c1"), 0};
                found = true;
            }
        }
    } else if (!gs.board.get_white() && s == stosq("e8")) {
        if (gs.board.get_cas_bs() && check_squares_for_castling(gs, stosq("f8"), stosq("g8"), SQUARE_SENTINEL)) {
            if (counter.has_space()) {
                moves[counter.inc()] = Move{stosq("e8"), stosq("g8"), 0};
                found = true;
            }
        }
        if (gs.board.get_cas_bl() && check_squares_for_castling(gs, stosq("d8"), stosq("c8"), stosq("b8"))) {
            if (counter.has_space()) {
                moves[counter.inc()] = Move{stosq("e8"), stosq("c8"), 0};
                found = true;
            }
        }
    }

    return found;
}

/**
 * Suggests plausible pawn moves. Moves forward to supported (not necessarily safe) squares
 * including double moves.
 */
void pawn_moves(const Gamestate & gs, const Square sq, Move * moves, IndexCounter & counter) {

    int
        x = get_x(sq),
        y = get_y(sq);

    const int FORWARD = gs.board.get_white() ? 1 : -1;

    // check one square in front; if it would be supported, move there
    if (gs.board.get(mksq(x, y + FORWARD)) == EMPTY) {
        if ((gs.board.get_white() && w_pawn_defence_count(gs, mksq(x, y + FORWARD)) > 0)
                || (!gs.board.get_white() && b_pawn_defence_count(gs, mksq(x, y + FORWARD)) > 0)) {
            if (counter.has_space()) {
                moves[counter.inc()] = Move{sq, mksq(x, y + FORWARD)};
            } else {
                // no more space, all done
                return;
            }
        }
    } else {
        // square in front is not empty, no moves
        return;
    }

    // check for double moves (always!)
    bool first_rank = gs.board.get_white() ? (y == 1) : (y == 6);
    if (first_rank && gs.board.get(mksq(x, y + FORWARD + FORWARD)) == EMPTY) {
        if (counter.has_space()) {
            moves[counter.inc()] = Move{sq, mksq(x, y + FORWARD + FORWARD)};
        } else {
            return;
        }
    }

}

/**
 * Looks for the best square to develop the given piece to. Although this is deterministic
 * up to the position of the pieces, it may return more than one square for all pieces which
 * are not knights.
 */
void develop_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & move_counter) {

    const Board & b = gs.board;

    bool is_white_piece = colour(b.get(ff->centre)) == WHITE;
    if (b.get_white() != is_white_piece) {
        return;
    }

    switch (type(b.get(ff->centre))) {
        case BISHOP:
            best_diag_squares(b, ff->centre, moves, move_counter, BISHOPS);
            return;
        case KNIGHT:
            best_knight_square(b, ff->centre, moves, move_counter);
            return;
        case ROOK:
            best_ortho_squares(b, ff->centre, moves, move_counter, ROOKS);
            return;
        case QUEEN:
            best_diag_squares(b, ff->centre, moves, move_counter, QUEENS);
            best_ortho_squares(b, ff->centre, moves, move_counter, QUEENS);
            return;
        case KING:
            castling_moves(gs, ff->centre, moves, move_counter);
            return;
        case PAWN:
            pawn_moves(gs, ff->centre, moves, move_counter);
            return;
        default:
            return;
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
void is_undeveloped_piece_hook(const Gamestate & gs, const Square centre, std::vector<FeatureFrame> & frames) {
    if (is_undeveloped_piece(gs, centre)) {
        frames.push_back(FeatureFrame{centre, SQUARE_SENTINEL, 0, 0});
    }
}
