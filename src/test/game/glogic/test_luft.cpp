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
        },
        LuftTestCase{
            // both kings blocked in with pieces
            "6k1/5ppp/8/8/8/8/5BPP/6K1 w - - 0 1",
            false,
            false
        },
        LuftTestCase{
            // white unblocked, black covered by bishop
            "6k1/5p1p/6p1/8/3B4/8/6PP/6K1 w - - 0 1",
            true,
            false
        },
        LuftTestCase{
            // white not on edge, black castled long with escape square covered
            "2kr1b1r/ppp2ppp/3q4/4N2n/3n4/2N4P/PPPPQPPK/R1B1R3 w - - 1 10",
            true,
            false
        },
        LuftTestCase{
            // both kings blocked in odd fashions on non-standard files
            "8/K7/5B2/1r6/1N6/8/8/1k6 w - - 0 1",
            false,
            false
        },
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
