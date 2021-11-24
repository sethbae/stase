#include "cands.h"
#include "../heur/heur.h"

/**
 * This method contains the logical checks of whether a second piece can be discovered
 * which can either be pinned or skewered from the second piece back to the first.
 * It checks, for example:
 *  - type, colour
 *  - pawns: weakly defended
 */
inline void detect_pin_skewer(const Gamestate & gs, const Square s, const Delta d, std::vector<FeatureFrame> & frames) {

    Square other_p_sq = first_piece_encountered(gs.board, s, d);

    // check we actually hit a piece
    if (is_sentinel(other_p_sq)) {
        return;
    }

    if (!val(s.x - d.dx, s.y - d.dy) || gs.board.get(s.x - d.dx, s.y - d.dy) != EMPTY) {
        // no pin in this direction is possible
        return;
    }

    Piece other_p = gs.board.get(other_p_sq);

    // only consider pawns which aren't 'structurally' defended
    if (type(other_p) == PAWN
        && control_count(gs.board, other_p_sq) != 0) {
        return;
    }

    // pin pieces of the same colour
    if (colour(other_p) != colour(gs.board.get(s))) {
        return;
    }

    /*
     * Here, we have found a pair of pieces: --k---r-
     * We already checked the first piece encountered for whether it can move in
     * the current direction or not (k, here). So a pin/skewer from that piece
     * to this piece is definitely on.
     * The reverse direction will be checked from the rook, not here. Those checks
     * should not pass!
     * The delta we want to put on the frame is the one coming back from the piece
     * we've found to the original piece, which is the opposite of the direction
     * we came in. So we flip dx and dy. The centre of the frame is the piece first
     * encountered - s - and the secondary is the other piece: other_p_sq.
     */

    frames.push_back(FeatureFrame{s, other_p_sq, -d.dx, -d.dy});
}

/**
 * Detects whether or not the given square contains a piece which can either be pinned or
 * be skewered. The centre of the frame will always be the first piece encountered during
 * the putative pin or skewer - in pins, that is the less valuable piece, in skewers it is
 * the more valuable. The frames will also contain a delta, which describes the line taken
 * from the second piece hit, back to the first piece.
 *
 * The format of the FeatureFrames is this:
 * centre: the square of the pinned/skewered piece
 * secondary: the square of the piece it could be pinned to / skewered against
 * conf_1: the x delta passing from the second piece to the first piece
 * conf_2: the y delta passing from the second piece to the first piece
 */
void find_pin_skewer_hook(const Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    bool check_ortho;
    bool check_diag;
    Piece p = gs.board.get(s);

    if (p == EMPTY) {
        return;
    }

    if (type(p) == PAWN) {
        // check only those pawns which currently hang in the balance
        if (control_count(gs.board, s) == 0) {
            check_ortho = true;
            check_diag = true;
        } else {
            return;
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
            detect_pin_skewer(gs, s, Delta{XD[dir], YD[dir]}, frames);
        }
    }

    if (check_diag) {
        for (int dir = DIAG_START; dir < DIAG_STOP; ++dir) {
            if (!val(s.x - XD[dir], s.y - YD[dir])
                || gs.board.get(s.x - XD[dir], s.y - YD[dir]) != EMPTY) {
                // no pin in this direction is possible
                continue;
            }
            detect_pin_skewer(gs, s, Delta{XD[dir], YD[dir]}, frames);
        }
    }

}
