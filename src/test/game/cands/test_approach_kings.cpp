#include "../../test.h"

const TestSet<ResponderTestCase> approach_kings_test_set = {
    "game-cands-approach-kings",
    {
        // white approaches
        ResponderTestCase {
            "8/6k1/8/8/4K3/8/8/8 w - - 0 1",
            {FeatureFrame{stosq("e4"), SQUARE_SENTINEL, 0, 0}},
            {"e4f5"}
        },
        // black approaches
        ResponderTestCase {
            "8/8/1K6/8/8/8/2k5/8 w - - 0 1",
            {FeatureFrame{stosq("c2"), SQUARE_SENTINEL, 0, 0}},
            {"c2b3"}
        },
        // white does not use unsafe square
        ResponderTestCase {
            "8/4b3/1K6/8/3p4/8/2k5/8 w - - 0 1",
            {FeatureFrame{stosq("b6"), SQUARE_SENTINEL, 0, 0}},
            {}
        },
        // black does not use occupied square
        ResponderTestCase {
            "8/8/8/8/3p1K2/2k5/8/8 w - - 0 1",
            {FeatureFrame{stosq("c3"), SQUARE_SENTINEL, 0, 0}},
            {}
        },
        // move along one axis when blocked
        ResponderTestCase {
            "8/8/4k3/8/3K4/8/8/8 w - - 0 1",
            {FeatureFrame{stosq("d4"), SQUARE_SENTINEL, 0, 0}},
            {"d4e4"}
        },
        // don't move out of diagonal opposition
        ResponderTestCase {
            "8/8/5k2/8/3K4/8/8/8 w - - 0 1",
            {FeatureFrame{stosq("f5"), SQUARE_SENTINEL, 0, 0}},
            {}
        }
    }
};

bool evaluate_approach_kings_test_case(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&approach_kings_resp, tc);
}

bool test_approach_kings() {
    return evaluate_test_set(&approach_kings_test_set, &evaluate_approach_kings_test_case);
}
