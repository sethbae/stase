#include "../../test.h"

const TestSet<HookTestCase> king_pinned_pieces_hook_test_set{
    "game-cands-king-pinned-pieces",
    {
        // two pawns pinned
        HookTestCase{
            "rnb1kbnr/pppppppp/4q3/1B6/8/8/PPPPPPPP/RNBQK1NR w KQkq - 0 1",
            {
                ExpectedFeatureFrame{"e2", "e6", 0, 1},
                ExpectedFeatureFrame{"d7", "b5", -1, -1}
            }
        },
        // bishop and rook
        HookTestCase{
            "8/5k2/5b2/8/3R1Q2/2K5/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"d4", "f6", 1, 1},
                ExpectedFeatureFrame{"f6", "f4", 0, -1}
            }
        },
        // queen pinned
        HookTestCase{
            "8/4k3/3r4/8/8/8/3Q4/3K4 w - - 0 1",
            {
                ExpectedFeatureFrame{"d2", "d6", 0, 1}
            }
        },
        // puzzle #1
        HookTestCase{
            "6k1/1pQ2p2/p2p2p1/3P2qn/1P2P1P1/5PK1/P1B5/8 w - - 1 38",
            {
                ExpectedFeatureFrame{"g4", "g5", 0, 1}
            }
        },
        // puzzle #2
        HookTestCase{
            "8/1p2k2p/2rrbp2/5Q2/8/P5P1/1P3P1P/1K2R3 b - - 0 28",
            {
                ExpectedFeatureFrame{"e6", "e1", 0, -1}
            }
        },
        // puzzle #3
        HookTestCase{
            "6k1/pp2np1p/3q4/1P2R3/8/4Q2P/P5PK/8 w - - 13 34",
            {
                ExpectedFeatureFrame{"e5", "d6", -1, 1}
            }
        },
        // puzzle #4
        HookTestCase{
            "3r4/ppbP2k1/3R3p/8/4N2p/5P2/1P4PK/8 w - - 2 33",
            {
                ExpectedFeatureFrame{"d6", "c7", -1, 1}
            }
        },
    }
};

bool evaluate_king_pinned_pieces_hook_test_case(const HookTestCase * tc) {
    return evaluate_hook_test_case(&king_pinned_pieces_hook, tc);
}

bool test_king_pinned_pieces_hook() {
    return evaluate_test_set(&king_pinned_pieces_hook_test_set, &evaluate_king_pinned_pieces_hook_test_case);
}
