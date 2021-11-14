#include "../../test.h"

const TestSet<ResponderTestCase> desperado_test_cases = {
        "game-cands-desperado-piece",
        {
            // does nothing if no captures are available
            ResponderTestCase {
                "8/8/8/4N3/8/8/8/8 w - - 0 1",
                {FeatureFrame{stosq("e5")}},
                {}
            },
            // does not capture piece of own colour
            ResponderTestCase {
                "8/8/8/3p4/2b5/8/8/8 b - - 0 1",
                {FeatureFrame{stosq("c4")}},
                {}
            },
            // captures most valuable piece
            ResponderTestCase {
                "3r4/8/8/1q1R1b2/8/8/3n4/8 w - - 0 1",
                {FeatureFrame{stosq("d5")}},
                {"d5b5"}
            },
            // captures defended piece of lower value if only option available
            ResponderTestCase {
                "1b6/2P5/1P6/8/8/8/8/8 b - - 0 1",
                {FeatureFrame{stosq("b8")}},
                {"b8c7"}
            },
            // puzzle #1
            ResponderTestCase {
                "4rrk1/6p1/pQ5p/1pb1P2n/8/3n1PB1/PP2N1PP/R4RK1 w - - 2 22",
                {FeatureFrame{stosq("b6")}},
                {"b6c5"}
            },
        }
};

bool evaluate_desperado_test_case(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&desperado_resp, tc);
}

bool test_desperado_piece() {
    return evaluate_test_set(&desperado_test_cases, &evaluate_desperado_test_case);
}
