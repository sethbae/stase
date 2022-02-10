#include "../../test.h"

const TestSet<ResponderTestCase> fork_responder_test_set{
    "game-cands-fork-responder",
    {
        // basics: bishop
        ResponderTestCase{
            "6r1/8/8/5B2/2r5/8/8/8 w - - 0 1",
            { FeatureFrame{stosq("c4"), stosq("g8"), sq_sentinel_as_int(), k + 1} },
            {"f5e6"}
        },
        // basics: rook
        ResponderTestCase{
            "1B4N1/8/8/5r2/8/8/8/8 w - - 0 1",
            { FeatureFrame{stosq("b8"), stosq("g8"), sq_sentinel_as_int(), k + 1} },
            {"f5f8"}
        },
        // bishop does not fork on unsafe square
        // rook does not fork on unsafe square

        // white pawn
        // black pawn

        // knight
        // knight not on unsafe square

        // king
        // king doesn't on unsafe square

        // queen plays normal fork

        // pieces don't respond to forks with value restrictions

        // queen plays queen fork 1
        // queen plays queen fork 2
    }
};

bool evaluate_fork_responder_test_case(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&play_fork_resp, tc);
}

bool test_fork_responder() {
    return evaluate_test_set(&fork_responder_test_set, &evaluate_fork_responder_test_case);
}
