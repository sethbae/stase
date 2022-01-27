#include "board.h"
#include "heur.h"
#include "../gamestate.hpp"

float __metrics::__development_metric(const Gamestate & gs) {

    int count = 0;

    const Ptype BACK_RANK[] = {
            ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
    };

    for (int x = 0; x < 8; ++x) {
        if (type(gs.board.get(x, 7)) == BACK_RANK[x]) {
            ++count;
        }
        if (type(gs.board.get(x, 0)) == BACK_RANK[x]) {
            --count;
        }
    }

    return ((float)count) / 4.0f;

}
