#include "cands.h"

/**
 * Checks for a pawn on the given square which can promote. Also checks for 3rd / 6th rank
 * pawns which can safely move forward. Writes a feature frame for each one it finds. Format is:
 * -centre: promotion square
 * -secondary: current square of pawn
 * -conf1: unused
 * -conf2: unused
 */
void can_promote_hook(Gamestate & gs, const Square s, std::vector<FeatureFrame> & frames) {

    Piece p = gs.board.get(s);

    if (type(p) != PAWN || gs.is_kpinned_piece(s)) {
        return;
    }

    if ((p == W_PAWN && s.y < 6)
            || (p == B_PAWN && s.y > 1)) {
        return;
    }

    const int FORWARD = (p == W_PAWN ? 1 : -1);
    Square temp = mksq(s.x, s.y + FORWARD);

    if (gs.board.get(temp) != EMPTY
        || would_be_unsafe_after(gs, temp, Move{s, temp})) {
        return;
    }

    frames.push_back(FeatureFrame{mksq(s.x, s.y + FORWARD), s});

}

/**
 * Pushes the pawn one square forward, if space allows.
 */
void promote_pawn(const Gamestate & gs, const FeatureFrame * ff, Move * m, IndexCounter & counter) {
    if (counter.has_space()) {
        m[counter.inc()] = Move{ff->secondary, ff->centre};
    }
}
