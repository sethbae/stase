#include "board.h"
#include "heur.h"

float development_metric(const Board & b) {

    int count = 0;

    const Ptype BACK_RANK[] = {
            ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
    };

    for (int x = 0; x < 8; ++x) {
        if (type(b.get(x, 7)) == BACK_RANK[x]) {
            ++count;
        }
        if (type(b.get(x, 0)) == BACK_RANK[x]) {
            --count;
        }
    }

    return ((float)count) / 4.0f;

}
