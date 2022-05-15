#include "glogic.h"
#include "../gamestate.hpp"

void find_pawn_moves_to(const Gamestate & gs, ptr_vec<Move> & moves, const Colour c, const Square target) {

    Square l_sq;
    Square r_sq;
    if (colour(gs.board.get(target)) == c) { return; }

    // captures onto the target square (including e-p)
    if (c == WHITE) {
        l_sq = mksq(target.x - 1, target.y - 1);
        r_sq = mksq(target.x + 1, target.y - 1);
    } else {
        l_sq = mksq(target.x - 1, target.y + 1);
        r_sq = mksq(target.x + 1, target.y + 1);
    }
    if (gs.board.get(target) != EMPTY || (gs.board.get_ep_exists() && equal(target, gs.board.get_ep_sq()))) {
        if (val(l_sq)) {
            Piece p = gs.board.get(l_sq);
            if (type(p) == PAWN && colour(p) == c) {
                moves.push(Move{l_sq, target, 0});
            }
        }
        if (val(r_sq)) {
            Piece p = gs.board.get(r_sq);
            if (type(p) == PAWN && colour(p) == c) {
                moves.push(Move{r_sq, target, 0});
            }
        }
    }

    // moves straight forward
    if (gs.board.get(target) == EMPTY) {
        Square behind_sq =
                mksq(
                        target.x,
                        target.y + (c == WHITE ? -1 : +1)
                );
        if (val(behind_sq)) {
            Piece p = gs.board.get(behind_sq);
            if (type(p) == PAWN && colour(p) == c) {
                moves.push(Move{behind_sq, target, 0});
            }

            // double move forward
            if (gs.board.get(behind_sq) == EMPTY) {
                if (c == WHITE && target.y == 3) {
                    Square behind2_sq = mksq(behind_sq.x, behind_sq.y - 1);
                    if (val(behind2_sq)) {
                        Piece p2 = gs.board.get(behind2_sq);
                        if (type(p2) == PAWN && colour(p2) == c) {
                            moves.push(Move{behind2_sq, target, 0});
                        }
                    }
                } else if (c != WHITE && target.y == 4) {
                    Square behind2_sq = mksq(behind_sq.x, behind_sq.y + 1);
                    if (val(behind2_sq)) {
                        Piece p2 = gs.board.get(behind2_sq);
                        if (type(p2) == PAWN && colour(p2) == c) {
                            moves.push(Move{behind2_sq, target, 0});
                        }
                    }
                }
            }
        }
    }
}
