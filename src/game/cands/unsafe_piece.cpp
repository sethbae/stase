#include "game.h"
#include "cands.h"

/**
 * Wraps the weak square logic in a hook which, if the square is weak, records a feature frame
 * including the value of the weakest attackers (min_w in conf_1, min_b in conf_2).
 * This hook only records weak squares which have unsafe pieces on them.
 */
void is_unsafe_piece_hook(const Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    if (gs.board.get(s) == EMPTY) { return; }

    SquareControlStatus ss = evaluate_square_status(gs, s);

    if (is_weak_status(gs, s, colour(gs.board.get(s)), ss)) {
        frames.push_back(FeatureFrame{s, SQUARE_SENTINEL, ss.min_w, ss.min_b});
    }

}
