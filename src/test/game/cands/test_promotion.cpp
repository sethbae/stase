#include "../../test.h"

const TestSet<HookTestCase> promotion_hook_test_set{
    "game-cands-promotion-hook",
    {
        HookTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {}
        },
        // basic prom #1
        HookTestCase{
            "8/6P1/8/8/8/8/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"g8", "g7", 0, 0}
            }
        },
        // basic prom #2
        HookTestCase{
            "8/8/8/8/8/8/3p4/8 b - - 0 1",
            {
                ExpectedFeatureFrame{"d1", "d2", 0, 0}
            }
        },
        // on attacked but safe square
        HookTestCase{
            "r7/5P2/8/8/5R2/8/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"f8", "f7", 0, 0}
            }
        },
        // not using unsafe square
        HookTestCase{
            "8/8/8/8/6r1/8/6p1/1R3R2 b - - 0 1",
            {}
        },
        // from 6th rank #1
        HookTestCase{
            "8/8/2P5/8/8/8/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"c7", "c6", 0, 0}
            }
        },
        // from 6th rank #2
        HookTestCase{
            "8/8/8/8/8/p7/8/8 b - - 0 1",
            {
                ExpectedFeatureFrame{"a2", "a3", 0, 0}
            }
        },
        // not when blocked #1
        HookTestCase{
            "2k5/2P5/8/8/5B2/8/8/8 w - - 0 1",
            {}
        },
        // not when blocked #2
        HookTestCase{
            "8/8/8/8/8/5R2/1K3p2/5r2 w - - 0 1",
            {}
        },
        // puzzle #1
        HookTestCase{
            "8/1P6/5k2/3K1p2/8/4P3/7p/8 w - - 0 49",
            {
                ExpectedFeatureFrame{"b8", "b7", 0, 0},
                ExpectedFeatureFrame{"h1", "h2", 0, 0},
            }
        }
    }
};

bool evaluate_promotion_hook_test_case(const HookTestCase * tc) {
    return evaluate_hook_test_case(&promotion_hook, tc);
}

bool test_promotion_hook() {
    return evaluate_test_set(&promotion_hook_test_set, &evaluate_promotion_hook_test_case);
}
