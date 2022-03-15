#include "../../test.h"

const TestSet<HookTestCase> pin_skewer_hook_test_set{
    "game-cands-pin-skewer-hook",
    {
        HookTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {}
        },
        // basic pin: bishop
        HookTestCase{
            "r7/3B4/8/3K1B2/8/8/8/7r w - - 0 1",
            {
                ExpectedFeatureFrame{"a8", "d8", b, k},
                ExpectedFeatureFrame{"a8", "a5", k, b},
                ExpectedFeatureFrame{"h1", "d1", k, b},
                ExpectedFeatureFrame{"h1", "h5", b, k},
            }
        },
        // basic pin: knight (doesn't use unsafe square)
        HookTestCase{
            "8/8/5kn1/4n3/8/8/8/7Q w - - 0 1",
            {
                ExpectedFeatureFrame{"h1", "a1", kn, k},
                ExpectedFeatureFrame{"h1", "h6", kn, k}
            }
        },
        // basic pin: rook
        HookTestCase{
            "5b2/8/4k1r1/8/8/2R5/1K6/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"f8", "g7", r, k},
                ExpectedFeatureFrame{"c3", "c6", k, r},
            }
        },
        // basic pin: queen
        HookTestCase{
            "8/8/1k6/1q6/8/3R4/1K6/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"d3", "b3", q, k},
            }
        },
        // basic pin: queen #2
        HookTestCase{
            "8/8/8/3Q4/4K3/7b/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"h3", "g2", k, q},
            }
        },
        //basic pin: pawn
        HookTestCase{
            "8/b7/3K4/8/5P2/8/3P4/1r4b1 w - - 0 1",
            {
                ExpectedFeatureFrame{"a7", "b8", k, p},
                ExpectedFeatureFrame{"b1", "d1", p, k},
                ExpectedFeatureFrame{"g1", "h2", p, k},
            }
        },
        // basic pin: not pawn
        HookTestCase{
            "6k1/2b2ppp/2r5/8/8/R7/2B2PPP/6K1 w - - 0 1",
            {}
        },
        // basic pin: not bishop
        HookTestCase{
            "2r2k2/4bbpp/8/n7/8/8/4B1PP/1R2BK2 w - - 0 1",
            {}
        },
        // basic pin: not rook
        HookTestCase{
            "1r6/k2r4/8/8/1r6/8/3R4/3K4 w - - 0 1",
            {}
        },
        // puzzle #1
        HookTestCase{
            "r5k1/1bp4p/1p3bp1/3q1p2/4p3/4P2P/3QBPPB/2R3K1 w - - 0 27",
            {
                ExpectedFeatureFrame{"e2", "c4", q, k},
                ExpectedFeatureFrame{"a8", "a2", q, b},
                ExpectedFeatureFrame{"d5", "a2", q, b},
            }
        },
        // puzzle #2
        HookTestCase{
            "2kr1br1/ppq2p2/2p1nBb1/4p2p/4P3/2N1QP1P/PPP1B1P1/R4RK1 b - - 0 17",
            {
                ExpectedFeatureFrame{"f8", "c5", q, k},
                ExpectedFeatureFrame{"c7", "b6", q, k},
            }
        },
    }
};

bool evaluate_pin_skewer_hook_test_case(const HookTestCase * tc) {
    return evaluate_hook_test_case(pin_skewer_hook, tc);
}

bool test_pin_skewer_hook() {
    return evaluate_test_set(&pin_skewer_hook_test_set, &evaluate_pin_skewer_hook_test_case);
}
