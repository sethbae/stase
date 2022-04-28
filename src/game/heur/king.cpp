#include <iostream>
using std::cout;

#include "board.h"
#include "heur.h"
#include "../gamestate.hpp"

/**
 * Returns zero if both sides have either castled or not castled, otherwise
 * +1 or -1 to indicate that only one side has castled.
 */
float __metrics::__castling_metric(const Gamestate & gs) {
    if (gs.b_cas == gs.w_cas) { return 0.0f; }
    return (gs.w_cas) ? 1.0f : -1.0f;
}

/**
 * Returns a score comparing the castling rights available to each side.
 */
float __metrics::__castling_rights_metric(const Gamestate & gs) {

    float w_score = 0.0f, b_score = 0.0f;

    if (gs.w_cas) {
        w_score = 2.0f;
    } else {
        if (gs.board.get_cas_ws()) { w_score += 0.85f; }
        if (gs.board.get_cas_wl()) { w_score += 0.65f; }
    }

    if (gs.b_cas) {
        b_score = 2.0f;
    } else {
        if (gs.board.get_cas_bs()) { b_score += 0.85f; }
        if (gs.board.get_cas_bl()) { b_score += 0.65f; }
    }

    return 0.5f * (w_score - b_score);
}

/**
 * Returns a score of how well the pawns defend the king on the given square.
 */
int pk_count(const Gamestate & gs, const bool use_white) {

    int score = 0;
    Square ksq = use_white ? gs.w_king : gs.b_king;
    Colour king_col = use_white ? WHITE : BLACK;
    int delta = use_white ? 1 : -1;
    int x = ksq.x;
    int y = ksq.y;

    // check for unusual king locations and return 0
    if (use_white) {
        if (x == 3 || x == 4 || y > 1) {
            return 0;
        }
    } else {
        if (x == 3 || x == 4 || y < 6) {
            return 0;
        }
    }

    Piece p;

    // pawns directly in front of the king
    if (x > 0) {
        p = gs.board.get(mksq(x - 1 , y + delta));
        if (type(p) == PAWN && colour(p) == king_col) {
            score += 5;
        }
    }

    p = gs.board.get(mksq(x, y + delta));
    if (type(p) == PAWN && colour(p) == king_col) {
        score += 5;
    }

    if (x < 7) {
        p = gs.board.get(mksq(x + 1 , y + delta));
        if (type(p) == PAWN && colour(p) == king_col) {
            score += 5;
        }
    }

    // pawns a square away
    if (x > 0) {
        p = gs.board.get(mksq(x - 1 , y + 2*delta));
        if (type(p) == PAWN && colour(p) == king_col) {
            score += 2;
        }
    }

    p = gs.board.get(mksq(x, y + 2*delta));
    if (type(p) == PAWN && colour(p) == king_col) {
        score += 2;
    }

    if (x < 7) {
        p = gs.board.get(mksq(x + 1 , y + 2*delta));
        if (type(p) == PAWN && colour(p) == king_col) {
            score += 2;
        }
    }

    return score;

}

float __metrics::__pawns_defend_king_metric(const Gamestate & gs) {
    // delegate to the pawn-king-counter (pk-count)
    return ((float)(pk_count(gs, true) - pk_count(gs, false))) / 8.0f;
}

/**
 * Returns a score representing the balance of control near the king.
 * Sums gamma control count on all adjacent squares to each king.
 */
float __metrics::__control_near_king_metric(const Gamestate & gs) {

    int score = 0;
    int wx = gs.w_king.x, wy = gs.w_king.y, bx = gs.b_king.x, by = gs.b_king.y;

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

    // black king
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

/**
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
        if ( p != EMPTY && can_move_in_direction(p, dir)) {
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
float __metrics::__king_exposure_metric(const Gamestate & gs) {
    return (((float)one_king_exposure(gs, gs.w_king)) + ((float)one_king_exposure(gs, gs.b_king))) / 6.0f;
}

float __metrics::__unconventional_king_metric(const Gamestate & gs) {
    Square w_ksq = gs.w_king;
    Square b_ksq = gs.b_king;
    bool w_conventional = equal(w_ksq, stosq("b1")) || equal(w_ksq, stosq("c1")) || equal(w_ksq, stosq("g1")) || equal(w_ksq, stosq("h1"));
    bool b_conventional = equal(b_ksq, stosq("b8")) || equal(b_ksq, stosq("c8")) || equal(b_ksq, stosq("g8")) || equal(b_ksq, stosq("h8"));
    if (w_conventional && !b_conventional) {
        return 1.0f;
    } else if (!w_conventional && b_conventional) {
        return -1.0f;
    } else {
        return 0.0f;
    }
}
