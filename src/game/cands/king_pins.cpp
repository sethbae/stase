#include "cands.h"

/**
 * Detects whether or not the given square contains a king to which pieces can be pinned.
 * That is, it will only execute on the square containing the king, and will write frames
 * for each piece which is pinned to it. The frames will contain a delta, which describes
 * the direction of travel from the king to that specific piece pinned to it in the given
 * frame. The format of the FeatureFrames is this:
 * centre: the square of the pinned piece
 * secondary: the square of the king it could be pinned to
 * conf_1: the x delta passing from the king to the piece
 * conf_2: the y delta passing from the king to the piece
 */
void find_kpinnable_hook(const Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    Piece king = gs.board.get(s);

    if (type(king) != KING) {
        return;
    }

    // fan out in all directions looking for pieces
    for (int i = ALL_DIRS_START; i < ALL_DIRS_STOP; ++i) {

        Delta d = {XD[i], YD[i]};
        Square pinned_sq = first_piece_encountered(gs.board, s, d);

        // check that we actually hit a piece
        if (is_sentinel(pinned_sq)) {
            continue;
        }

        Piece pinned = gs.board.get(pinned_sq);

        // check colour, pawn
        if (colour(gs.board.get(pinned_sq)) != colour(king)
            || type(pinned) == PAWN) {
            continue;
        }

        // don't pin pieces which can move in that direction, unless they're queens
        if (type(pinned) != QUEEN && can_move_in_direction(gs.board.get(pinned_sq), direction_of_delta(d))) {
            continue;
        }

        frames.push_back(FeatureFrame{pinned_sq, s, d.dx, d.dy});

    }

}
