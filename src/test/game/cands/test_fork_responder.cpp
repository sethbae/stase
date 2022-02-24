#include "../../test.h"

const TestSet<ResponderTestCase> fork_responder_test_set{
    "game-cands-fork-responder",
    {
        // basics: bishop
        ResponderTestCase{
            "6r1/8/8/5B2/2r5/8/8/8 w - - 0 1",
            { FeatureFrame{stosq("c4"), stosq("g8"), k + 1, sq_sentinel_as_int()} },
            {"f5e6"}
        },
        // basics: rook
        ResponderTestCase{
            "1B4N1/8/8/5r2/8/8/8/8 w - - 0 1",
            { FeatureFrame{stosq("b8"), stosq("g8"), k + 1, sq_sentinel_as_int()} },
            {"f5f8"}
        },
        // bishop does not fork on unsafe square
        ResponderTestCase{
            "8/8/8/2b3R1/8/8/3R1P2/8 w - - 0 1",
            { FeatureFrame{stosq("d2"), stosq("g5"), k + 1, sq_sentinel_as_int()} },
            {}
        },
        // rook does not fork on unsafe square
        ResponderTestCase{
            "8/8/8/2bb4/6R1/8/2n5/8 w - - 0 1",
            { FeatureFrame{stosq("c4"), stosq("c2"), k + 1, sq_sentinel_as_int()} },
            {}
        },
        // white pawn
        ResponderTestCase{
            "8/8/8/2n1q3/8/4P3/3P4/8 w - - 0 1",
            { FeatureFrame{stosq("c5"), stosq("e5"), sqtoi(stosq("d4")), sqtoi(stosq("d2"))} },
            {"d2d4"}
        },
        // black pawn
        ResponderTestCase{
            "8/8/5p2/4P3/3R1R2/8/8/8 w - - 0 1",
            { FeatureFrame{stosq("d4"), stosq("f4"), sqtoi(stosq("e5")), sqtoi(stosq("f6"))} },
            {"f6e5"}
        },
        // knight
        ResponderTestCase{
            "r3k3/5r2/8/3N4/8/8/7B/8 w - - 0 1",
            { FeatureFrame{stosq("a8"), stosq("e8"), sqtoi(stosq("c7")), sqtoi(stosq("d5"))}},
            {"d5c7"}
        },
        // king
        ResponderTestCase{
            "4k3/8/3K1p2/5p2/8/8/8/8 w - - 0 1",
            { FeatureFrame{stosq("f6"), stosq("f5"), sqtoi(stosq("e6")), sqtoi(stosq("d6"))}},
            {"d6e6"}
        },
        // queen plays normal fork
        ResponderTestCase{
            "5r2/8/8/8/4Q3/r7/8/8 w - - 0 1",
            { FeatureFrame{stosq("a3"), stosq("f8"), k + 1, sq_sentinel_as_int()}},
            {"e4e7", "e4b4"}
        },
        // pieces don't respond to forks with value restrictions
        ResponderTestCase{
            "8/1b6/4R3/8/8/1R6/P5q1/8 w - - 0 1",
            { FeatureFrame{stosq("b3"), stosq("e6"), r, sq_sentinel_as_int()} },
            {"b7d5"}
        },
        // queen plays queen fork
        ResponderTestCase{
            "8/2q5/8/6B1/8/8/8/4K3 w - - 0 1",
            {
                FeatureFrame{stosq("e1"), stosq("g5"), sqtoi(stosq("a5")), sqtoi(stosq("c7"))},
                FeatureFrame{stosq("e1"), stosq("g5"), sqtoi(stosq("g3")), sqtoi(stosq("c7"))}
            },
            {"c7a5", "c7g3"}
        }
    }
};

bool evaluate_fork_responder_test_case(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&play_fork_resp, tc);
}

bool test_fork_responder() {
    return evaluate_test_set(&fork_responder_test_set, &evaluate_fork_responder_test_case);
}
