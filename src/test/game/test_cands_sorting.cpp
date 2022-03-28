#include "game.h"
#include "../test.h"


bool __test_cand_sorting() {
    std::vector<Move> moves;
    for (int i = 0; i < 10; ++i) {
        Move m = MOVE_SENTINEL;
        m.set_score(i);
        moves.push_back(m);
    }

    CandSet c;
    c.critical = moves;
    c.order_list(CRITICAL);

    for (int i = 0; i < 10; ++i) {
        if (c.critical[i].get_score() != (9 - i)) {
            return false;
        }
    }

    return true;
}

bool test_cands_sorting() {
    return evaluate_test_function("game-cands-sorting", &__test_cand_sorting);
}
