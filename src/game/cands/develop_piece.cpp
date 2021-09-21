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
 * Looks for the best square to develop the given piece to. Although this is deterministic
 * up to the position of the pieces, it may return more than one square for all pieces which
 * are not knights.
 */
void develop_piece(const Board & b, const FeatureFrame * ff, Move * moves, IndexCounter & move_counter) {

    bool white_piece = colour(b.get(ff->centre)) == WHITE;
    if (b.get_white() != white_piece) {
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
        default:
            return;
    }

}
