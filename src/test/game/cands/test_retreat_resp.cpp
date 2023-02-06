#include "../../test.h"

const TestSet<ResponderTestCase> retreat_test_cases = {
    "game-cands-retreat-piece",
    {
        // white piece retreats to safe square
        ResponderTestCase{
            "8/8/3p4/2p1p3/3B4/2P5/8/8 w - - 0 1",
            {FeatureFrame{stosq("d4"), SQUARE_SENTINEL, 0, 0}},
            {"d4e3", "d4f2", "d4g1"}
        },
        // black piece retreats to safe square
        ResponderTestCase{
            "8/rp6/1P6/P7/8/8/8/4B3 b - - 0 1",
            {FeatureFrame{stosq("a7"), SQUARE_SENTINEL, 0, 0}},
            {"a7a6", "a7a8"}
        },
        // retreats with capture
        ResponderTestCase{
            "8/8/8/8/8/8/1pq5/N7 w - - 0 1",
            {FeatureFrame{stosq("a1"), SQUARE_SENTINEL, 0, 0}},
            {"a1c2"}
        },
        // does not retreat to unsafe square #1
        ResponderTestCase{
            "2b1k3/1ppq1p2/1p2p3/7p/3N2p1/8/8/8 w - - 0 1",
            {FeatureFrame{stosq("d4"), SQUARE_SENTINEL, 0, 0}},
            {"d4b3", "d4c2", "d4e2"}
        },
        // does not retreat to unsafe square #2
        ResponderTestCase{
            "8/8/8/8/8/b1N5/1qPB4/1R1Q4 b - - 0 1",
            {FeatureFrame{stosq("b2"), SQUARE_SENTINEL, 0, 0}},
            {}
        },
        // retreats in multiple directions
        ResponderTestCase{
            "2k5/3R4/1K6/8/8/8/8/8 w - - 0 1",
            {FeatureFrame{stosq("d7"), SQUARE_SENTINEL, 0, 0}},
            {"d7e7", "d7f7", "d7g7", "d7d6", "d7d5"}
        }
    }
};

bool evaluate_retreat_test_case(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&retreat_resp, tc);
}

bool test_retreat_piece() {
    return evaluate_test_set(&retreat_test_cases, &evaluate_retreat_test_case);
}
