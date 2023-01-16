#include "../../test.h"
#include "../../../game/gamestate.hpp"

struct LuftTestCase {
    const std::string fen;
    const bool white_luft_expected;
    const bool black_luft_expected;
};

const TestSet<LuftTestCase> luft_test_set{
    "game-glogic-luft",
    {
        LuftTestCase{
            starting_fen(),
            false,
            false
        }
    }
};

bool evaluate_luft_test_case(const LuftTestCase * tc) {
    const Gamestate gs(tc->fen);
    return has_luft(gs, WHITE) == tc->white_luft_expected
        && has_luft(gs, BLACK) == tc->black_luft_expected;
}

bool test_luft() {
    return evaluate_test_set(&luft_test_set, &evaluate_luft_test_case);
}
