#include "../heur/heur.h"
#include "cands.h"
#include "../../../include/stase/game.h"
#include "../gamestate.hpp"
#include "responder.hpp"

/**
 * Generates moves which capture a weak piece. Searches outwards from the target square looking for
 * pieces of the right colour which can capture the piece, and suggests these moves in order of
 * increasing strength (it starts with the weakest/least valuable piece).
 *
 * Reads the information needed from the given FeatureFrame and writes moves to the pointer given,
 * working from idx up until the end index is reached.
 */
int capture_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {

    const Square s = ff->centre;
    const Board & b = gs.board;
    const int local_reset_point = idx;
    const Colour capturing_colour = opposite_colour(gs.board.get(s));
    const int weakest_defender = (capturing_colour == WHITE ? ff->conf_2 : ff->conf_1);
    const bool totally_undefended = weakest_defender > piece_value(W_KING);
    const Piece captured_piece = gs.board.get(s);

    int min_value_seen = NOT_ATTACKED_AT_ALL;
    int x, y;
    Square temp;

    // go through the delta pairs entailing each sliding direction
    MoveType dir = DIAG;
    for (int i = 0; i < 8; ++i) {

        if (i == 4) {
            dir = ORTHO;
        }

        int x_inc = XD[i], y_inc = YD[i];
        x = get_x(s) + x_inc, y = get_y(s) + y_inc;

        // and go in that direction
        while (val(temp = mksq(x, y))) {

            Piece p = b.get(temp);
            Delta mov_dir = get_delta_between(temp, s);

            if ((p != EMPTY) && can_move_in_direction(p, dir)
                    && colour(p) == capturing_colour
                    && !gs.is_kpinned_piece(temp, mov_dir)) {

                // piece of the right colour which can move in the right dir: check value
                int val = piece_value(p);
                if (val < piece_value(b.get(s)) || val <= weakest_defender) {
                    if (val < min_value_seen) {

                        // new lowest value; reset and add to the list
                        min_value_seen = val;
                        idx = local_reset_point;

                        Move m{temp, s, 0};
                        m.set_score(capture_piece_score(totally_undefended, p, captured_piece));
                        moves[idx++] = m;

                    } else if (val == min_value_seen) {

                        // equal lowest value; append
                        if (idx < end) {
                            Move m{temp, s, 0};
                            m.set_score(capture_piece_score(totally_undefended, p, captured_piece));
                            moves[idx++] = m;
                        }
                        // note that here and below, we cannot shortcut and return early. We
                        // may later discover a less valuable piece, or a further piece of equal value.
                        else {}
                    }
                }
                // no x-rays!
                break;
            } else if (p != EMPTY) {
                // blocking piece: abort
                break;
            }

            // square was empty, continue
            x += x_inc;
            y += y_inc;

        }

    }

    x = get_x(s);
    y = get_y(s);

    // knights
    int kn_val = piece_value(W_KNIGHT);
    if (min_value_seen >= kn_val) {
        if (kn_val < piece_value(b.get(s)) || weakest_defender >= kn_val) {
            for (int i = 0; i < 8; ++i) {
                if (val(temp = mksq(x + XKN[i], y + YKN[i]))
                    && (type(b.get(temp)) == KNIGHT)
                    && (colour(b.get(temp)) == capturing_colour)
                    && !gs.is_kpinned_piece(temp, KNIGHT_DELTA)) {

                    if (kn_val < min_value_seen) {
                        // new lowest value; reset and add to the list
                        min_value_seen = kn_val;
                        idx = local_reset_point;
                        Move m{temp, s, 0};
                        m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                        moves[idx++] = m;
                    } else if (kn_val == min_value_seen) {
                        // equal lowest value; append
                        if (idx < end) {
                            Move m{temp, s, 0};
                            m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                            moves[idx++] = m;
                        }
                    }
                }
            }
        }
    }

    // kings
    int k_val = piece_value(W_KING);
    if (min_value_seen >= k_val && weakest_defender >= k_val) {
        for (int i = 0; i < 8; ++i) {
            if (val(temp = mksq(x + XD[i], y + YD[i]))
                && (type(b.get(temp)) == KING)
                && (colour(b.get(temp)) == capturing_colour)
                && would_be_safe_king_square(gs, s, capturing_colour)) {
                if (k_val < min_value_seen) {
                    // new lowest value; reset and add to the list
                    min_value_seen = k_val;
                    idx = local_reset_point;
                    Move m{temp, s, 0};
                    m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                    moves[idx++] = m;
                } else if (k_val == min_value_seen) {
                    // equal lowest value; append
                    if (idx < end) {
                        Move m{temp, s, 0};
                        m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                        moves[idx++] = m;
                    }
                }
            }
        }
    }

    // pawns
    int pawn_val = piece_value(W_PAWN);
    if (val(temp = mksq(x + 1, y + 1)) && (b.get(temp) == B_PAWN) && (capturing_colour == BLACK)
            && !gs.is_kpinned_piece(temp, get_delta_between(temp, s))) {
        // IMPLICIT: pawns are always <= weakest defender
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            idx = local_reset_point;
            Move m{temp, s, 0};
            m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
            moves[idx++] = m;
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (idx < end) {
                Move m{temp, s, 0};
                m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                moves[idx++] = m;
            }
        }
    }
    if (val(temp = mksq(x - 1, y + 1)) && (b.get(temp) == B_PAWN) && (capturing_colour == BLACK)
            && !gs.is_kpinned_piece(temp, get_delta_between(temp, s))) {
        // IMPLICIT: pawns are always <= weakest defender
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            idx = local_reset_point;
            Move m{temp, s, 0};
            m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
            moves[idx++] = m;
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (idx < end) {
                Move m{temp, s, 0};
                m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                moves[idx++] = m;
            }
        }
    }
    if (val(temp = mksq(x + 1, y - 1)) && (b.get(temp) == W_PAWN) && (capturing_colour == WHITE)
             && !gs.is_kpinned_piece(temp, get_delta_between(temp, s))) {
        // IMPLICIT: pawns are always <= weakest defender
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            idx = local_reset_point;
            Move m{temp, s, 0};
            m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
            moves[idx++] = m;
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (idx < end) {
                Move m{temp, s, 0};
                m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                moves[idx++] = m;
            }
        }
    }
    if (val(temp = mksq(x - 1, y - 1)) && (b.get(temp) == W_PAWN) && (capturing_colour == WHITE)
            && !gs.is_kpinned_piece(temp, get_delta_between(temp, s))) {
        // IMPLICIT: pawns are always <= weakest defender
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            idx = local_reset_point;
            Move m{temp, s, 0};
            m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
            moves[idx++] = m;
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (idx < end) {
                Move m{temp, s, 0};
                m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                moves[idx++] = m;
            }
        }
    }

    // en passant
    if (gs.board.get_ep_exists() && equal(s, gs.board.get_ep_pawn_square())) {

        // square on one side
        if (s.x > 0) {
            temp = mksq(s.x - 1, s.y);
            if (type(gs.board.get(temp)) == PAWN
                && colour(gs.board.get(temp)) == capturing_colour
                && !gs.is_kpinned_piece(temp, get_delta_between(temp, s))) {
                // IMPLICIT: pawns are always <= weakest defender
                if (pawn_val < min_value_seen) {
                    // new lowest value; reset and add to the list
                    min_value_seen = pawn_val;
                    idx = local_reset_point;
                    Move m{temp, gs.board.get_ep_sq(), 0};
                    m.set_ep();
                    m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                    moves[idx++] = m;
                } else if (pawn_val == min_value_seen) {
                    // equal lowest value; append
                    if (idx < end) {
                        Move m{temp, gs.board.get_ep_sq(), 0};
                        m.set_ep();
                        m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                        moves[idx++] = m;
                    }
                }
            }
        }

        // and on the other
        if (s.x < 7) {
            temp = mksq(s.x + 1, s.y);
            if (type(gs.board.get(temp)) == PAWN
                && colour(gs.board.get(temp)) == capturing_colour
                && !gs.is_kpinned_piece(temp, get_delta_between(temp, s))) {
                // IMPLICIT: pawns are always <= weakest defender
                if (pawn_val < min_value_seen) {
                    // new lowest value; reset and add to the list
                    min_value_seen = pawn_val;
                    idx = local_reset_point;
                    Move m{temp, gs.board.get_ep_sq(), 0};
                    m.set_ep();
                    m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                    moves[idx++] = m;
                } else if (pawn_val == min_value_seen) {
                    // equal lowest value; append
                    if (idx < end) {
                        Move m{temp, gs.board.get_ep_sq(), 0};
                        m.set_ep();
                        m.set_score(capture_piece_score(totally_undefended, b.get(temp), captured_piece));
                        moves[idx++] = m;
                    }
                }
            }
        }
    }
    return idx;
}

const Responder capture_resp{
    "capture",
    &capture_piece
};
