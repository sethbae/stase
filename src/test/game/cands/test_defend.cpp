#include "../../test.h"
#include "../../../game/cands/cands.h"

#include "board.h"
#include "game.h"

const TestSet<ResponderTestCase> defend_test_set = {
        "game-cands-defend-square",
        std::vector<ResponderTestCase>{
            // defend e4 in one of three ways
            ResponderTestCase{
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                std::vector<FeatureFrame>{
                    FeatureFrame{stosq("e4"), 0, none, none}
                },
                {"d2d3", "f2f3", "b1c3"}
            },
            // can't defend e6
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
//            ResponderTestCase{
//                    "",
//                    std::vector<FeatureFrame>{
//                            FeatureFrame{stosq("e4"), 0, 0, 0}
//                    },
//                    {""}
//            },
//            ResponderTestCase{
//                    "",
//                    std::vector<FeatureFrame>{
//                            FeatureFrame{stosq("e4"), 0, 0, 0}
//                    },
//                    {""}
//            },
//            ResponderTestCase{
//                    "",
//                    std::vector<FeatureFrame>{
//                            FeatureFrame{stosq("e4"), 0, 0, 0}
//                    },
//                    {""}
//            },
//            ResponderTestCase{
//                    "",
//                    std::vector<FeatureFrame>{
//                            FeatureFrame{stosq("e4"), 0, 0, 0}
//                    },
//                    {""}
//            },
//            ResponderTestCase{
//                    "",
//                    std::vector<FeatureFrame>{
//                            FeatureFrame{stosq("e4"), 0, 0, 0}
//                    },
//                    {""}
//            },
//            ResponderTestCase{
//                    "",
//                    std::vector<FeatureFrame>{
//                            FeatureFrame{stosq("e4"), 0, 0, 0}
//                    },
//                    {""}
//            },
//            ResponderTestCase{
//                    "",
//                    std::vector<FeatureFrame>{
//                            FeatureFrame{stosq("e4"), 0, 0, 0}
//                    },
//                    {""}
//            },
        }
};


bool evaluate_test_case_defend(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&defend_square, tc);
}

void test_defend_square() {
    evaluate_test_set(&defend_test_set, &evaluate_test_case_defend);
}
