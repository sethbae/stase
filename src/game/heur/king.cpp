#include <iostream>
using std::cout;

#include "board.h"
#include "game.h"
#include "heur.h"

/*
 * Returns a score of how well the pawns defend the king on the given square.
 * no checking on king beyond colour
 */
int pk_count(const Board & b, const Square s) {

    int score = 0;
    int x = get_x(s);
    int y = get_y(s);
    Colour king_col = colour(b.get(s));
    int delta = (king_col == WHITE) ? 1 : -1;

    // check for unusual king locations and return 0
    if (king_col == WHITE) {
        if (x == 3 || x == 4 || y > 1) {
            return 0;
        }
    } else if (king_col == BLACK) {
        if (x == 3 || x == 4 || y < 6) {
            return 0;
        }
    } else {
        return 0;
    }

    Piece p;

    // pawns directly in front of the king
    if (x > 0) {
        p = b.get(mksq(x - 1 , y + delta));
        if (type(p) == PAWN && colour(p) == king_col) {
            score += 5;
        }
    }

    p = b.get(mksq(x, y + delta));
    if (type(p) == PAWN && colour(p) == king_col) {
        score += 5;
    }

    if (x < 7) {
        p = b.get(mksq(x + 1 , y + delta));
        if (type(p) == PAWN && colour(p) == king_col) {
            score += 5;
        }
    }

    // pawns a square away
    if (x > 0) {
        p = b.get(mksq(x - 1 , y + 2*delta));
        if (type(p) == PAWN && colour(p) == king_col) {
            score += 2;
        }
    }

    p = b.get(mksq(x, y + 2*delta));
    if (type(p) == PAWN && colour(p) == king_col) {
        score += 2;
    }

    if (x < 7) {
        p = b.get(mksq(x + 1 , y + 2*delta));
        if (type(p) == PAWN && colour(p) == king_col) {
            score += 2;
        }
    }

    return score;

}

float pawns_defend_king_metric(const Gamestate & gs) {

    Square wking = Square{0, 0};
    Square bking = Square{0, 0};

    // find kings
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(mksq(x, y)) == W_KING) {
                wking = mksq(x, y);
            } else if (gs.board.get(mksq(x, y)) == B_KING) {
                bking = mksq(x, y);
            }
        }
    }

//    cout << "White: " << pk_count(b, wking) << "\n";
//    cout << "Black: " << pk_count(b, bking) << "\n";

    // delegate to the pawn-king-counter (pk-count)
    return ((float)(pk_count(gs.board, wking) - pk_count(gs.board, bking))) / 8.0f;
}

/*
 * Returns a score representing the balance of control near the king.
 * sums gamma control count on all adjacent squares to each king.
 */
float control_near_king_metric(const Gamestate & gs) {

    int score = 0;
    int wx = 0, wy = 0, bx = 0, by = 0;

    // find kings
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(mksq(x, y)) == W_KING) {
                wx = x;
                wy = y;
            } else if (gs.board.get(mksq(x, y)) == B_KING) {
                bx = x;
                by = y;
            }
        }
    }

    Square sq;

    // white king
    if (val(sq = mksq(wx + 1, wy + 1)))
        score += gamma_control(gs.board, sq);
    if (val(sq = mksq(wx + 1, wy + 0)))
        score += gamma_control(gs.board, sq);
    if (val(sq = mksq(wx + 1, wy - 1)))
        score += gamma_control(gs.board, sq);
    if (val(sq = mksq(wx + 0, wy + 1)))
        score += gamma_control(gs.board, sq);
    if (val(sq = mksq(wx + 0, wy - 1)))
        score += gamma_control(gs.board, sq);
    if (val(sq = mksq(wx - 1, wy + 1)))
        score += gamma_control(gs.board, sq);
    if (val(sq = mksq(wx - 1, wy + 0)))
        score += gamma_control(gs.board, sq);
    if (val(sq = mksq(wx - 1, wy - 1)))
        score += gamma_control(gs.board, sq);

    // gs.boardlack king
    if (val(sq = mksq(bx + 1, by + 1)))
        score -= gamma_control(gs.board, sq);
    if (val(sq = mksq(bx + 1, by + 0)))
        score -= gamma_control(gs.board, sq);
    if (val(sq = mksq(bx + 1, by - 1)))
        score -= gamma_control(gs.board, sq);
    if (val(sq = mksq(bx + 0, by + 1)))
        score -= gamma_control(gs.board, sq);
    if (val(sq = mksq(bx + 0, by - 1)))
        score -= gamma_control(gs.board, sq);
    if (val(sq = mksq(bx - 1, by + 1)))
        score -= gamma_control(gs.board, sq);
    if (val(sq = mksq(bx - 1, by + 0)))
        score -= gamma_control(gs.board, sq);
    if (val(sq = mksq(bx - 1, by - 1)))
        score -= gamma_control(gs.board, sq);

    if (score > 15) { score = 15; }
    else if (score < -15) { score = -15; }

    return ((float) score) / 15.0f;

}

/*
 * for the given location and movement, this returns the control count along the path
 * traced until a pawn is reach, subject to:
 *      - the path having length >= 2
 *      - the control being unfavourable, see below
 * if this is unfavourable for the colour given, ie x > 0 for BLACK and x < 0 for WHITE,
 * then it is returned - otherwise 0 is returned
 */
int exposure_count(const Gamestate & gs, const Square s, Delta d, MoveType dir, Colour col) {

    Square sq = s;
    int control_count = 0, length = 0;
    Piece p;

    while (val(sq) && type(p = gs.board.get(sq)) != PAWN) {
        if (can_move_in_direction(p, dir)) {
            control_count += (colour(p) == WHITE ? 1 : -1);
        }
        ++length;
        // move to next iteration
        sq.x += d.dx;
        sq.y += d.dy;
    }

    /*
        scores only count if
            - the section has length 2 or above
            - the control count is unfavourable
    */
    if (length >= 2 &&
    ((control_count > 0 && col == BLACK) || (control_count < 0 && col == WHITE))) {
        return control_count;
    } else {
        return 0;
    }

}

/*
 * Returns an integer: how exposed is the king along nearby files and diagonals,
 * obeying the normal black/white positive/negative favourability.
 * Only returns an unfavourable score, as per exposure_count
 * Pass the king's square!
 */
int one_king_exposure(const Gamestate & gs, const Square s) {

    Colour col = colour(gs.board.get(s));
    int score = 0;

    Square ahead = mksq(get_x(s), get_y(s) + 1);
    Square behind = mksq(get_x(s), get_y(s) - 1);
    Square centre = s;
    Square left = mksq(get_x(s) - 1, get_y(s));
    Square right = mksq(get_x(s) + 1, get_y(s));

    Square start_points[] = { ahead, behind, centre, left, right };

    for (int i = 0; i < 3; ++i) {
        score += exposure_count(gs, start_points[i], Delta{1, 0}, ORTHO, col);
        score += exposure_count(gs, start_points[i], Delta{-1, 0}, ORTHO, col);
        score += exposure_count(gs, start_points[i], Delta{1, 1}, DIAG, col);
        score += exposure_count(gs, start_points[i], Delta{-1, 1}, DIAG, col);
        score += exposure_count(gs, start_points[i], Delta{1, -1}, DIAG, col);
        score += exposure_count(gs, start_points[i], Delta{-1, -1}, DIAG, col);
    }

    for (int i = 3; i < 5; ++i) {
        score += exposure_count(gs, start_points[i], Delta{0, 1}, ORTHO, col);
        score += exposure_count(gs, start_points[i], Delta{0, -1}, ORTHO, col);
    }

    return score;
}

/**
 * Combines two calls to one_king_exposure into a metric.
 */
float king_exposure_metric(const Gamestate & gs) {

    Square wking = Square{0, 0}, bking = Square{0, 0};

    // find kings
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(mksq(x, y)) == W_KING) {
                wking = mksq(x, y);
            } else if (gs.board.get(mksq(x, y)) == B_KING) {
                bking = mksq(x, y);
            }
        }
    }

    // cout << "White scores: " << one_king_exposure(b, wking) << "\n";
    // cout << "Black scores: " << one_king_exposure(b, bking) << "\n";

    return (((float)one_king_exposure(gs, wking)) + ((float)one_king_exposure(gs, bking))) / 6.0f;

}
