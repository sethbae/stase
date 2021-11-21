#include "board.h"
#include "heur.h"

float development_metric(const Board & b) {

    int count = 0;

    const Ptype BACK_RANK[] = {
            ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
    };

    for (Square sq = stosq("a8"); val(sq); inc_x(sq)) {
        if (type(b.get(sq)) == BACK_RANK[get_x(sq)]) {
            ++count;
        }
    }

    for (Square sq = stosq("a1"); val(sq); inc_x(sq)) {
        if (type(b.get(sq)) == BACK_RANK[get_x(sq)]) {
            --count;
        }
    }

    return ((float)count) / 4.0f;

}
