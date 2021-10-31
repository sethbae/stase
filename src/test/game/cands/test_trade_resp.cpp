#include "../../test.h"

const TestSet<ResponderTestCase> trade_test_cases = {
    "game-cands-trade-piece",
    {
        // straightforward capture
        ResponderTestCase{
            "8/3r2p1/5b2/8/3B4/8/8/8 w - - 0 1",
            {FeatureFrame{stosq("d4")}},
            {"d4f6"}
        },
        // doesn't capture undefended, weaker piece (this is not a trade)
        ResponderTestCase{
            "8/8/8/4b3/8/2P5/4R3/8 b - - 0 1",
            {FeatureFrame{stosq("e5")}},
            {}
        },
        // does capture more valuable piece
        ResponderTestCase{
            "8/8/8/4n3/5q2/3N4/8/8 w - - 0 1",
            {FeatureFrame{stosq("d3")}},
            {"d3f4"}
        },
        // white rook trades
        ResponderTestCase{
            "8/1p1n4/2r5/2R5/8/8/5Q2/8 w - - 0 1",
            {FeatureFrame{stosq("c5")}},
            {"c5c6"}
        },
        // black rook trades
        ResponderTestCase{
            "8/4p3/3r4/8/8/6B1/2RR4/8 b - - 0 1",
            {FeatureFrame{stosq("d6")}},
            {"d6d2"}
        },
        // white queen trades
        ResponderTestCase{
            "8/8/5q2/8/4n3/8/3R1Q2/8 w - - 0 1",
            {FeatureFrame{stosq("f2")}},
            {"f2f6"}
        },
        // black queen trades
        ResponderTestCase{
            "8/8/5q2/8/4N3/8/3R1Q2/8 b - - 0 1",
            {FeatureFrame{stosq("f6")}},
            {"f6f2"}
        },
        // puzzle #1
        ResponderTestCase{
            "2rb2k1/3n1ppp/3N4/1P2p3/qP2Pn2/B3QN1P/5PP1/2R3K1 b - - 0 25",
            {FeatureFrame{stosq("c8")}},
            {"c8c1"}
        }
    }
};

bool evaluate_trade_test_case(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&trade_resp, tc);
}

void test_trade_piece() {
    evaluate_test_set(&trade_test_cases, &evaluate_trade_test_case);
}
