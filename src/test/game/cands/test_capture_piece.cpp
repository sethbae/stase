#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../test.h"
#include "../../../game/heur/heur.h"

const TestSet<ResponderTestCase> cap_test_set = {
        "game-cands-capture-piece",
        {
                // starting position has none
                ResponderTestCase{
                        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        std::vector<FeatureFrame>{
                            FeatureFrame{stosq("e4"), 0, 0, 0}
                        },
                        {},
                },
                // doesn't capture #1
                ResponderTestCase{
                        "8/6b1/8/8/3N4/2P5/8/8 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d4"), 0, p, b}
                        },
                        {}
                },
                // doesn't capture #2
                ResponderTestCase{
                        "3r4/3q4/8/3P4/8/1B6/8/8 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d5"), 0, b, q}
                        },
                        {}
                },
                // prefers weaker piece #1
                ResponderTestCase{
                        "8/8/8/3r4/2P1P3/8/8/3Q4 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d5"), 0, p, r}
                        },
                        {"c4d5", "e4d5"}
                },
                // prefers weaker piece #2
                ResponderTestCase{
                        "3r4/8/8/5b2/8/3R4/8/8 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d3"), 0, none, b}
                        },
                        {"f5d3"}
                },
                // knight takes
                ResponderTestCase{
                        "2q5/8/1N6/5b2/8/3r4/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("c8"), 0, kn, b}
                        },
                        {"b6c8"}
                },
                // bishop doesn't take
                ResponderTestCase{
                        "8/6p1/5p2/3N4/8/8/8/B7 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("f6"), 0, kn, p}
                        },
                        {}
                },
                // rook takes
                ResponderTestCase{
                        "8/1r3q2/8/8/8/1Q6/8/8 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("b3"), 0, none, r}
                        },
                        {"b7b3"}
                },
                // queen takes
                ResponderTestCase{
                        "8/5q2/8/8/8/1Q6/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("f7"), 0, q, none}
                        },
                        {"b3f7"}
                },
                // takes better but defended piece
                ResponderTestCase{
                        "8/8/8/8/1k2K3/1rqR3R/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("c3"), 0, r, r}
                        },
                        {"d3c3"}
                },
                // has two options #1
                ResponderTestCase{
                        "8/8/8/8/1k2K3/3R1b1R/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("f3"), 0, r, none}
                        },
                        {"d3f3", "h3f3"}
                },
                // has two options #2
                ResponderTestCase{
                        "8/8/5N2/8/4q3/2N2k2/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("e4"), 0, kn, none}
                        },
                        {"c3e4", "f6e4"}
                },
        }
};

bool evaluate_test_case_capture(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&capture_piece, tc);
}

void test_capture_piece() {
    evaluate_test_set(&cap_test_set, &evaluate_test_case_capture);
}
