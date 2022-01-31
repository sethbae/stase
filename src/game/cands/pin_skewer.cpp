#include "cands.h"
#include "../gamestate.hpp"
#include "responder.hpp"

/**
 * Returns the final empty square on the trajectory and board given.
 */
inline Square find_line_end(const Gamestate & gs, int x, int y, Delta d) {
    int line_end_x = x, line_end_y = y;
    while (val(line_end_x - d.dx, line_end_y - d.dy)
           && gs.board.get(line_end_x - d.dx, line_end_y - d.dy) == EMPTY) {
        line_end_x -= d.dx;
        line_end_y -= d.dy;
    }
    return mksq(line_end_x, line_end_y);
}

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
inline bool detect_pin_skewer(Gamestate & gs, const Square s, const Delta d) {

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
        && control_count(gs.board, other_p_sq) != 0) {
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
    Square line_end = find_line_end(gs, s.x, s.y, d);

    // check for pieces which can move onto the line of empty squares
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {

            Piece p = gs.board.get(x, y);

            if (colour(p) == colour(gs.board.get(s))
                || !can_move_in_direction(p, dir)) {
                continue;
            }

            // if we are pinning with a queen, then there may be multiple squares to check
            if (type(p) == QUEEN) {

                std::vector<Square> squares_on_line =
                        squares_piece_can_reach_on_line(gs.board, mksq(x, y), s, line_end);

                for (int i = 0; i < squares_on_line.size(); ++i) {
                    Move pin_move = Move{mksq(x, y), squares_on_line[i], 0};
                    if (move_is_safe(gs, pin_move)
                            && !gs.is_kpinned_piece(mksq(x, y), get_delta_between(pin_move.from, pin_move.to))) {

                        bool result = gs.add_frame(
                                pin_skewer_hook.id,
                                FeatureFrame{
                                    mksq(x,y),
                                    squares_on_line[i],
                                    piece_value(gs.board.get(s)),
                                    piece_value(other_p)
                                }
                        );
                        if (!result) { return false; }

                    }
                }

            } else {

                // bishop / rook has at most one square available to it

                Square pin_from_square =
                        square_piece_can_reach_on_line(gs.board, mksq(x, y), s, line_end);

                if (is_sentinel(pin_from_square)) {
                    continue;
                }

                Move pin_move = Move{mksq(x, y), pin_from_square, 0};
                if (move_is_safe(gs, pin_move)
                        && !gs.is_kpinned_piece(mksq(x, y), get_delta_between(pin_move.from, pin_move.to))) {

                    bool result = gs.add_frame(
                            pin_skewer_hook.id,
                            FeatureFrame{
                                mksq(x, y),
                                pin_from_square,
                                piece_value(gs.board.get(s)),
                                piece_value(other_p)
                            }
                    );
                    if (!result) { return false; }

                }
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
        if (control_count(gs.board, s) == 0) {
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

const Hook pin_skewer_hook{
    "pin-skewer",
    4,
    &find_pin_skewer_hook
};

/**
 * This responder plays the move as described in the given FeatureFrame.
 */
void pin_or_skewer_piece(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {
    if (counter.has_space()){
        Move m = Move{ff->centre, ff->secondary};
        m.set_score(
            pin_skewer_score(
                piece_value(gs.board.get(ff->centre)),
                ff->conf_1,
                ff->conf_2
            )
        );
        moves[counter.inc()] = m;
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
bool identify_king_pinned_pieces_hook(Gamestate & gs, const Square s) {

    Piece king = gs.board.get(s);

    if (type(king) != KING) {
        return true;
    }

    for (int i = ALL_DIRS_START; i < ALL_DIRS_STOP; ++i) {

        Delta d{XD[i], YD[i]};
        Square pinned_sq = gs.first_piece_encountered(s, d);

        if (is_sentinel(pinned_sq) || colour(gs.board.get(pinned_sq)) != colour(king)) {
            continue;
        }

        Square pinner_sq = gs.first_piece_encountered(pinned_sq, d);

        if (is_sentinel(pinner_sq)) {
            continue;
        }

        if  (colour(gs.board.get(pinner_sq)) == colour(king)
                || !can_move_in_direction(gs.board.get(pinner_sq), d)) {
            continue;
        }

        gs.add_kpinned_piece(pinned_sq, d);
        bool result = gs.add_frame(king_pinned_pieces_hook.id, FeatureFrame{pinned_sq, pinner_sq, d.dx, d.dy});
        if (!result) { return false; }
    }
    return true;
}

const Hook king_pinned_pieces_hook {
    "king-pinned-pieces",
    5,
    &identify_king_pinned_pieces_hook
};

const Responder pin_skewer_resp{
    "pin-skewer",
    &pin_or_skewer_piece
};
