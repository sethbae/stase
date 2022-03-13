#include "../../test.h"

const TestSet<HookTestCase> discovered_hook_test_set{
    "game-cands-discovered-hook",
    {
        HookTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {}
        },
        // basic RBq (rook undefended)
        HookTestCase{
            "4q3/8/8/8/8/8/4B3/4R3 w - - 0 1",
            {
                ExpectedFeatureFrame{"e2", "e8", sqtoi(stosq("e1")), 1}
            }
        },
        // basic RBq (rook defended)
        HookTestCase{
            "4q3/8/8/8/8/8/4B3/1R2R3 w - - 0 1",
            {
                ExpectedFeatureFrame{"e2", "e8", sqtoi(stosq("e1")), 0}
            }
        },
        // basic Rbr
        HookTestCase{
            "8/8/R2b1r2/8/8/8/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"d6", "a6", sqtoi(stosq("f6")), 1}
            }
        },
        // BNp (no check required)
        HookTestCase{
            "8/5p2/8/8/2N5/1B6/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"c4", "f7", sqtoi(stosq("b3")), 0}
            }
        },
        // bnB (check required)
        HookTestCase{
            "8/8/8/3b4/4r3/8/8/7B w - - 0 1",
            {
                ExpectedFeatureFrame{"e4", "h1", sqtoi(stosq("d5")), 1}
            }
        },
        // RQq (check required) TODO: here the only defender is the discoverer, so check is not registered
//        HookTestCase{
//            "3q4/8/8/8/3Q4/8/8/3R4 w - - 0 1",
//            {
//                ExpectedFeatureFrame{"d4", "d8", sqtoi(stosq("d1")), 1}
//            }
//        },
        // rkB (no check required)
        HookTestCase{
            "3r4/3k4/8/4B3/8/8/8/3B4 w - - 0 1",
            {
                ExpectedFeatureFrame{"d7", "d1", sqtoi(stosq("d8")), 0}
            }
        },
        // BPq (no check required)
        HookTestCase{
            "8/8/1q6/8/8/4P3/4RB2/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"e3", "b6", sqtoi(stosq("f2")), 0}
            }
        },
        // doesn't 'discover' with piece that can move in that direction
        HookTestCase{
            "8/8/8/P1r1r3/8/8/8/8 w - - 0 1",
            {}
        },
    }
};

bool evaluate_discovered_hook_test_case(const HookTestCase * tc) {
    return evaluate_hook_test_case(&discovered_hook, tc);
}

bool test_discovered_hook() {
    return evaluate_test_set(&discovered_hook_test_set, &evaluate_discovered_hook_test_case);
}
