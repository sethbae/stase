#include "board.h"
#include "cands.h"

const Ptype BACK_RANK[] = {
        ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
};

/**
 * Checks if the given square contains an undeveloped piece.
 * @param ff the feature frame to record details in, if so.
 */
bool is_undeveloped_piece(const Gamestate & gs, Square centre) {
    if (get_y(centre) == 0 || get_y(centre) == 7) {
        return type(gs.board.get(centre)) == BACK_RANK[get_x(centre)];
    } else {
        return false;
    }
}

/**
 * Wraps the is_undeveloped_piece method in a hook which, if there is an undeveloped piece,
 * pushes a new FeatureFrame onto the list.
 */
void is_undeveloped_piece_hook(const Gamestate & gs, const Square centre, std::vector<FeatureFrame> & frames) {
    if (is_undeveloped_piece(gs, centre)) {
        frames.push_back(FeatureFrame{centre, SQUARE_SENTINEL, 0, 0});
    }
}
