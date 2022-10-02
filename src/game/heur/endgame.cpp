#include "board.h"
#include "heur.h"
#include "../glogic/glogic.h"
#include "../gamestate.hpp"

bool pole_position(const bool white, const Square pawn_sq, const Square king_sq) {
    if (white) {
        return (king_sq.x == pawn_sq.x - 1 || king_sq.x == pawn_sq.x + 1)
               && (king_sq.y == pawn_sq.y + 1 || king_sq.y == pawn_sq.y + 2);
    } else {
        return (king_sq.x == pawn_sq.x - 1 || king_sq.x == pawn_sq.x + 1)
               && (king_sq.y == pawn_sq.y - 1 || king_sq.y == pawn_sq.y - 2);
    }
}

float prom_squares_score(const Gamestate & gs, const Square s, const Piece p) {

    const bool white = (colour(p) == WHITE);
    const Square own_k_sq = (white ? gs.w_king : gs.b_king);
    const Square other_k_sq = (white ? gs.b_king : gs.w_king);

    if (white && s.y < 4) { return 0.0f; }
    if (!white && s.y > 3) { return 0.0f; }

    if (pole_position(white, s, own_k_sq)) {
        return pole_position(white, s, other_k_sq)
           ? 0.75f
           : 1.0f;
    }
    return 0.0f;
}

float __metrics::__promotion_squares(const Gamestate & gs) {

    float score = 0.0f;

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {

            const Square s = mksq(x, y);
            const Piece p = gs.board.get(s);

            if (type(p) == PAWN) {
                score +=
                    (colour(p) == WHITE)
                        ? + prom_squares_score(gs, s, p)
                        : - prom_squares_score(gs, s, p);
            }
        }
    }

    return score;
}
