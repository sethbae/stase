#include "cands.h"

/**
 * Checks for a pawn on the given square which can promote. Also checks for 3rd / 6th rank
 * pawns which can safely move forward. Writes a feature frame for each one it finds. Format is:
 * -centre: current square of pawn
 * -secondary: promotion square
 * -conf1: unused
 * -conf2: unused
 */
void can_promote_hook(Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    Piece p = gs.board.get(s);

    if (type(p) != PAWN) {
        return;
    }

    if ((p == W_PAWN && s.y < 5)
            || (p == B_PAWN && s.y > 2)) {
        return;
    }

    const int FORWARD = (p == W_PAWN ? 1 : -1);
    Square temp = mksq(s.x, s.y + FORWARD);

    if (gs.board.get(temp) != EMPTY
        || would_be_unsafe_after(gs, temp, Move{s, temp, 0})
        || gs.is_kpinned_piece(s, get_delta_between(s, temp))) {
        return;
    }

    frames.push_back(FeatureFrame{s, mksq(s.x, s.y + FORWARD), 0, 0});

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
