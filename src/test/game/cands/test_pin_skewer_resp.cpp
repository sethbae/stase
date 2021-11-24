#include "../../test.h"

const TestSet<ResponderTestCase> pin_skewer_resp_test_set{
    "game-cands-pin-skewer-resp",
    {
            // finds bishop pin
            ResponderTestCase{
                "8/8/2b5/8/8/8/2R5/1K6 w - - 0 1",
                {
                    FeatureFrame{stosq("c2"), stosq("b1"), 1, 1}
                },
                {"c6e4"}
            },
            // finds bishop skewer
            ResponderTestCase{
                "8/q7/7B/2k5/8/8/8/8 w - - 0 1",
                {
                    FeatureFrame{stosq("c5"), stosq("a7"), 1, -1}
                },
                {"h6e3"}
            },
            // finds rook pin
            ResponderTestCase{
                "7R/8/8/8/8/3kq3/8/7R w - - 0 1",
                {
                    FeatureFrame{stosq("e3"), stosq("d3"), 1, 0}
                },
                {"h1h3", "h8h3"}
            },
            // finds rook skewer
            ResponderTestCase{
                "8/8/3r4/8/8/3k4/8/7R w - - 0 1",
                {
                        FeatureFrame{stosq("d3"), stosq("d6"), 0, -1}
                },
                {"h1d1"}
            },
            // finds queen pin
            ResponderTestCase{
                "8/2q5/8/2R5/3K4/8/8/8 w - - 0 1",
                {
                    FeatureFrame{stosq("c5"), stosq("d4"), -1, 1}
                },
                {"c7b6", "c7a7"}
            },
            // finds queen skewer
            ResponderTestCase{
                "Q7/8/8/K7/8/6k1/8/3q4 w - - 0 1",
                {
                    FeatureFrame{stosq("a5"), stosq("a8"), 0, -1}
                },
                {"d1a1"}
            },
            // doesn't pin from unsafe square (bishop)
            ResponderTestCase{
                "8/8/3b4/8/3R1P2/2K5/8/8 w - - 0 1",
                {
                    FeatureFrame{stosq("d4"), stosq("c3"), -1, -1}
                },
                {}
            },
            // doesn't skewer from unsafe square (rook)
            ResponderTestCase{
                "1r6/8/8/8/8/2K4R/8/8 w - - 0 1",
                {
                    FeatureFrame{stosq("c3"), stosq("h3"), -1, 0}
                },
                {}
            },
            // doesn't pin from unsafe square (queen)
            ResponderTestCase{
                "5k2/1n6/3n4/8/8/8/2Q5/8 w - - 0 1",
                {
                    FeatureFrame{stosq("d6"), stosq("f8"), -1, -1}
                },
                {}
            },
            // puzzle #1
            ResponderTestCase{
                "2kr1br1/ppq2p2/2p1nBb1/4p2p/4P3/2N1QP1P/PPP1B1P1/R4RK1 b - - 0 17",
                {
                    FeatureFrame{stosq("e3"), stosq("g1"), -1, 1}
                },
                {"f8c5", "c7b6"}
            },
    }
};

bool evaluate_pin_skewer_resp_test_case(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&pin_skewer_resp, tc);
}

bool test_pin_skewer_resp() {
    return evaluate_test_set(&pin_skewer_resp_test_set, &evaluate_pin_skewer_resp_test_case);
}
