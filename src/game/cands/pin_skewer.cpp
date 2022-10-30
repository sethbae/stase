#include "cands.h"
#include "../gamestate.hpp"
#include "responder.hpp"

/**
 * This method contains the logical checks of whether a second piece can be discovered
 * which can either be pinned or skewered from the second piece back to the first.
 * It checks, type, colour etc and notably that a pawn is not defended by any other pawn.
 * Passing these, an enemy piece is then sought which can actually pin the piece from a safe square.
 * If so, FeatureFrames are written (per piece) with the following format:
 * - centre: the square of a piece able to play a pin or skewer
 * - secondary: the square on which it can play it
 * - conf1: the value of the piece which is being pinned or skewered
 * - conf2: the value of the piece which it is being pinned or skewered to
 */
bool detect_pin_skewer(Gamestate & gs, const Square s, const Delta d) {

    Square other_p_sq = gs.first_piece_encountered(s, d);

    // check we actually hit a piece
    if (is_sentinel(other_p_sq)) {
        return true;
    }

    if (!val(s.x - d.dx, s.y - d.dy) || gs.board.get(s.x - d.dx, s.y - d.dy) != EMPTY) {
        // no pin in this direction is possible
        return true;
    }

    Piece other_p = gs.board.get(other_p_sq);

    // only consider pawns which aren't 'structurally' defended
    if (type(other_p) == PAWN
        && gs.control_cache->get_control_count(other_p_sq) != 0) {
        return true;
    }

    // pin pieces of the same colour
    if (colour(other_p) != colour(gs.board.get(s))) {
        return true;
    }

    /*
     * Here, we have found a prospective pair of pieces, eg --k---r-
     * Next, we find enemy pieces which could in fact pin or skewer what we've found.
     */

    MoveType dir = direction_of_delta(d);

    for (int x = s.x - d.dx, y = s.y - d.dy;  val(x, y); x -= d.dx, y -= d.dy) {

        Square dest = mksq(x, y);
        if (gs.board.get(x, y) != EMPTY) { break; }

        for (int i = 0; i < 8; ++i) {

            Delta move_delta = D[i];
            if (parallel(move_delta, d)) { continue; }

            Square from = gs.first_piece_encountered(dest, move_delta);
            if (is_sentinel(from)) { continue; }

            Piece p = gs.board.get(from);
            if (colour(p) == colour(gs.board.get(s))
                || !can_move_in_direction(p, dir)
                || !can_move_in_direction(p, move_delta)) {
                continue;
            }

            Move pin_move{from, dest, 0};
            if (gs.is_kpinned_piece(from, move_delta) || !move_is_safe(gs, pin_move)) {
                continue;
            }

            if (!gs.add_frame(
                    pin_skewer_hook.id,
                    FeatureFrame{
                        from,
                        dest,
                        piece_value(gs.board.get(s)),
                        piece_value(other_p)
                    })) {
                return false;
            }
        }
    }

    return true;
}

/**
 * Detects whether or not the given square contains a piece which can either be pinned or
 * be skewered.
 * The format of the FeatureFrames is this:
 * - centre: the square of a piece able to play a pin or skewer
 * - secondary: the square on which it can play it
 * - conf1: the value of the piece which is being pinned or skewered
 * - conf2: the value of the piece which it is being pinned or skewered to
 */
bool find_pin_skewer_hook(Gamestate & gs, const Square s) {

    bool check_ortho;
    bool check_diag;
    Piece p = gs.board.get(s);

    if (p == EMPTY) {
        return true;
    }

    if (type(p) == PAWN) {
        // check only those pawns which currently hang in the balance
        if (gs.control_cache->get_control_count(s) == 0) {
            check_ortho = true;
            check_diag = true;
        } else {
            return true;
        }
    } else {
        // any king or queen, and pieces which can't move in the pin direction
        check_ortho = (piece_value(p) >= piece_value(QUEEN) || !can_move_in_direction(p, ORTHO));
        check_diag = (piece_value(p) >= piece_value(QUEEN) || !can_move_in_direction(p, DIAG));
    }

    if (check_ortho) {
        for (int dir = ORTHO_START; dir < ORTHO_STOP; ++dir) {
            if (!val(s.x - XD[dir], s.y - YD[dir])
                    || gs.board.get(s.x - XD[dir], s.y - YD[dir]) != EMPTY) {
                // no pin in this direction is possible
                continue;
            }
            bool result = detect_pin_skewer(gs, s, Delta{XD[dir], YD[dir]});
            if (!result) { return false; }
        }
    }

    if (check_diag) {
        for (int dir = DIAG_START; dir < DIAG_STOP; ++dir) {
            if (!val(s.x - XD[dir], s.y - YD[dir])
                || gs.board.get(s.x - XD[dir], s.y - YD[dir]) != EMPTY) {
                // no pin in this direction is possible
                continue;
            }
            bool result = detect_pin_skewer(gs, s, Delta{XD[dir], YD[dir]});
            if (!result) { return false; }
        }
    }
    return true;
}

/**
 * This responder plays the move as described in the given FeatureFrame.
 */
int pin_or_skewer_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {
    if (idx < end) {
        Move m = Move{ff->centre, ff->secondary, 0};
        m.set_score(
            pin_skewer_score(
                piece_value(gs.board.get(ff->centre)),
                ff->conf_1,
                ff->conf_2
            )
        );
        moves[idx++] = m;
    }
    return idx;
}

/**
 * These are detected on startup by the gamestate class, which writes the FeatureFrames in the following format.
 * The format of the feature frames is:
 * centre: the pinned piece
 * secondary: the piece which pins it
 * conf_1: dx of the pin direction
 * conf_2: dy of the pin direction
 */
bool identify_king_pinned_pieces_hook([[maybe_unused]] Gamestate & gs, [[maybe_unused]] const Square s) {
    // intentional stub
    return true;
}

const Hook king_pinned_pieces_hook {
    "king-pinned-pieces",
    5,
    &identify_king_pinned_pieces_hook
};

const Hook pin_skewer_hook{
        "pin-skewer",
        4,
        &find_pin_skewer_hook
};

const Responder pin_skewer_resp{
    "pin-skewer",
    &pin_or_skewer_piece
};
