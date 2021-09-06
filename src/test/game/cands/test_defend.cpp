#include "../../test.h"
#include "../../../game/cands/cands.h"

#include "board.h"
#include "game.h"

const TestSet<ResponderTestCase> defend_test_set = {
        "game-cands-defend-square",
        std::vector<ResponderTestCase>{
            // white defends e4 in one of three ways
            ResponderTestCase{
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                std::vector<FeatureFrame>{
                    FeatureFrame{stosq("e4"), 0, none, none}
                },
                {"d2d3", "f2f3", "b1c3"}
            },
            // white can't defend e6
            ResponderTestCase{
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("e6"), 0, none, none}
                    },
                    {}
            },
            // black defends e5
            ResponderTestCase{
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("e5"), 0, none, none}
                    },
                    {"f7f6", "d7d6", "b8c6"}
            },
            // bishop defends
            ResponderTestCase{
                    "6Q1/8/8/8/8/8/8/5B2 w - - 0 1",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("g8"), 0, none, none}
                    },
                    {"f1c4"}
            },
            // knights defend
            ResponderTestCase{
                    "8/2n1n3/8/8/3p4/8/8/8 b - - 0 1",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("d4"), 0, none, none}
                    },
                    {"e7c6", "c7e6", "e7f5", "c7b5"}
            },
            // queen defends
            ResponderTestCase{
                    "8/7p/6p1/5pP1/2PP4/3Q4/3K4/8 w - - 0 1",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("g5"), 0, b, none}
                    },
                    {"d3e3", "d3g3"}
            },
            // rook defends
            ResponderTestCase{
                    "r7/8/8/3p4/8/8/8/8 b - - 0 1",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("d5"), 0, none, none}
                    },
                    {"a8a5", "a8d8"}
            },
            // pawn defends
            ResponderTestCase{
                    "8/3p4/1p6/8/2p5/8/8/8 b - - 0 1",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("c4"), 0, none, none}
                    },
                    {"b6b5", "d7d5"}
            },
            // king defends
            ResponderTestCase{
                    "8/8/4P3/8/3K4/8/8/8 w - - 0 1",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("e6"), 0, none, none}
                    },
                    {"d4d5", "d4e5"}
            },
            // medley #1
            ResponderTestCase{
                    "rn1qk2r/1p2bppp/p2pbn2/4p3/4P3/1NN2P2/PPPB2PP/R1Q1KB1R w KQkq - 2 9",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("b4"), 0, none, none}
                    },
                    {"a2a3", "c3d5"}
            },
            // medley #2
            ResponderTestCase{
                    "rn2k2r/1p2bppp/3pb3/4p3/4P3/1NN2P2/PPPB2PP/R1Q1KB1R b KQkq - 2 9",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("b6"), 0, none, none}
                    },
                    {"a8a6", "b8d7", "e7d8"}
            },
            // doesn't defend with piece on same square #1
            ResponderTestCase{
                "r7/8/8/8/8/8/8/8 b - - 0 1",
                std::vector<FeatureFrame>{
                        FeatureFrame{stosq("a8"), 0, none, none}
                },
                {}
            },
            // doesn't defend with piece on same square #2
            ResponderTestCase{
                    "8/8/8/8/8/3K4/8/8 w - - 0 1",
                    std::vector<FeatureFrame>{
                            FeatureFrame{stosq("d3"), 0, none, none}
                    },
                    {}
            }
        }
};


bool evaluate_test_case_defend(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&defend_square, tc);
}

void test_defend_square() {
    evaluate_test_set(&defend_test_set, &evaluate_test_case_defend);
}
