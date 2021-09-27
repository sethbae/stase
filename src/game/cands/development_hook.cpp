#include "board.h"
#include "cands.h"

const Ptype BACK_RANK[] = {
        ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
};

/**
 * Checks if the given square contains an undeveloped piece.
 * @param ff the feature frame to record details in, if so.
 */
bool is_undeveloped_piece(const Gamestate & gs, Square centre, FeatureFrame * ff) {
    if (get_y(centre) == 0 || get_y(centre) == 7) {
        return type(gs.board.get(centre)) == BACK_RANK[get_x(centre)];
    } else {
        return false;
    }
}
