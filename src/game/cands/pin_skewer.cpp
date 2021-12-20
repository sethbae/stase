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
void find_pin_skewer_hook(Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

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

/**
 * This responder attempts to pin or skewer the piece as described in the feature frame. It does not
 * do this using unsafe squares.
 */
void pin_or_skewer_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {

    const Delta d{(SignedByte)ff->conf_1, (SignedByte)ff->conf_2};
    MoveType dir = direction_of_delta(d);

    // find the line of empty squares along which we can pin/skewer
    int line_end_x = ff->centre.x, line_end_y = ff->centre.y;
    while (val(line_end_x + d.dx, line_end_y + d.dy)
            && gs.board.get(line_end_x + d.dx, line_end_y + d.dy) == EMPTY) {
        line_end_x += d.dx;
        line_end_y += d.dy;
    }

    Square line_end = mksq(line_end_x, line_end_y);

    // check for pieces which can move onto the line of empty squares
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {

            Piece p = gs.board.get(x, y);

            if (colour(p) == colour(gs.board.get(ff->centre))
                || !can_move_in_direction(p, dir)) {
                continue;
            }

            // if we are pinning with a queen, then there may be multiple squares to check
            if (type(p) == QUEEN) {

                std::vector<Square> squares_on_line =
                        squares_piece_can_reach_on_line(gs.board, mksq(x, y), ff->centre, line_end);

                for (int i = 0; i < squares_on_line.size(); ++i) {
                    Move pin_move = Move{mksq(x, y), squares_on_line[i], 0};
                    if (!would_be_unsafe_after(gs, squares_on_line[i], pin_move)
                            && !gs.is_kpinned_piece(mksq(x, y), get_delta_between(pin_move.from, pin_move.to))) {
                        if (counter.has_space()) {
                            Move m{mksq(x, y), squares_on_line[i], 0};
                            m.set_score(pin_skewer_score(gs.board.get(ff->centre)));
                            moves[counter.inc()] = m;
                        } else {
                            return;
                        }
                    }
                }

            } else {

                // bishop / rook has at most one square available to it

                Square pin_from_square =
                        square_piece_can_reach_on_line(gs.board, mksq(x, y), ff->centre, line_end);

                if (is_sentinel(pin_from_square)) {
                    continue;
                }

                Move pin_move = Move{mksq(x, y), pin_from_square, 0};
                if (!would_be_unsafe_after(gs, pin_from_square, pin_move)
                        && !gs.is_kpinned_piece(mksq(x, y), get_delta_between(pin_move.from, pin_move.to))) {
                    if (counter.has_space()) {
                        Move m{mksq(x, y), pin_from_square, 0};
                        m.set_score(pin_skewer_score(gs.board.get(ff->centre)));
                        moves[counter.inc()] = m;
                    } else {
                        return;
                    }
                }
            }

        }
    }
}

/**
 * Detects pieces which in the current position are already pinned. Writes a feature frame for each such
 * piece it finds. This will only operate on a square containing a king.
 * The format of the feature frames is:
 * centre: the pinned piece
 * secondary: the piece which pins it
 * conf_1: dx of the pin direction
 * conf_2: dy of the pin direction
 */
void identify_king_pinned_pieces_hook(Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    Piece king = gs.board.get(s);

    if (type(king) != KING) {
        return;
    }

    for (int i = ALL_DIRS_START; i < ALL_DIRS_STOP; ++i) {

        Delta d{XD[i], YD[i]};
        Square pinned_sq = first_piece_encountered(gs.board, s, d);

        if (is_sentinel(pinned_sq) || colour(gs.board.get(pinned_sq)) != colour(king)) {
            continue;
        }

        Square pinner_sq = first_piece_encountered(gs.board, pinned_sq, d);

        if (is_sentinel(pinner_sq)) {
            continue;
        }

        if  (colour(gs.board.get(pinner_sq)) == colour(king)
                || !can_move_in_direction(gs.board.get(pinner_sq), d)) {
            continue;
        }

        frames.push_back(FeatureFrame{pinned_sq, pinner_sq, d.dx, d.dy});
        gs.add_kpinned_piece(pinned_sq, d);
    }
}
