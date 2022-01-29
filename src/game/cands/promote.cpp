#include "cands.h"
#include "../gamestate.hpp"
#include "responder.hpp"

/**
 * Checks for a pawn on the given square which can promote. Also checks for 3rd / 6th rank
 * pawns which can safely move forward. Writes a feature frame for each one it finds. Format is:
 * -centre: current square of pawn
 * -secondary: promotion square
 * -conf1: unused
 * -conf2: unused
 */
bool can_promote_hook(Gamestate & gs, const Square s) {

    Piece p = gs.board.get(s);

    if (type(p) != PAWN) {
        return true;
    }

    if ((p == W_PAWN && s.y < 5)
            || (p == B_PAWN && s.y > 2)) {
        return true;
    }

    const int FORWARD = (p == W_PAWN ? 1 : -1);
    Square temp = mksq(s.x, s.y + FORWARD);

    if (gs.board.get(temp) != EMPTY
        || weak_square(gs, temp, colour(gs.board.get(temp)), Move{s, temp, 0})
        || gs.is_kpinned_piece(s, get_delta_between(s, temp))) {
        return true;
    }

    return gs.add_frame(promotion_hook.id, FeatureFrame{s, mksq(s.x, s.y + FORWARD), 0, 0});

}

/**
 * Pushes the pawn one square forward, if space allows.
 */
void promote_pawn(const Gamestate & gs, const FeatureFrame * ff, Move * m, IndexCounter & counter) {
    if (counter.has_space()) {
        Move move{ff->centre, ff->secondary, 0};
        move.set_score(promotion_score());
        m[counter.inc()] = move;
    }
}

const Hook promotion_hook{
    "promotion",
    6,
    &can_promote_hook
};

const Responder promotion_resp{
    "promotion",
    &promote_pawn
};
