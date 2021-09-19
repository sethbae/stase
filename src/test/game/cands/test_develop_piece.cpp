#include "board.h"
#include "game.h"
#include "../../test.h"

#include <vector>

const TestSet<ResponderTestCase> develop_test_cases = {
        "game-cands-develop-piece",
        {
                // Be2, Bg2
                ResponderTestCase{
                        "5b2/8/8/8/8/8/8/5B2 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("f1"), 0, 0, 0}
                        },
                        { "f1e2", "f1g2" }
                },
                // Be7, Bg7
                ResponderTestCase{
                        "5b2/8/8/8/8/8/8/5B2 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("f8"), 0, 0, 0}
                        },
                        { "f8e7", "f8g7" }
                },
                // Nf3
                ResponderTestCase{
                        "6n1/8/8/8/8/8/8/6N1 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("g1"), 0, 0, 0}
                        },
                        { "g1f3" }
                },
                // Nf6
                ResponderTestCase{
                        "6n1/8/8/8/8/8/8/6N1 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("g8"), 0, 0, 0}
                        },
                        { "g8f6" }
                },
                // Rc1
                ResponderTestCase{
                        "r7/p7/8/8/8/8/P7/R7 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("a1"), 0, 0, 0}
                        },
                        { "a1c1" }
                },
                // Rc8
                ResponderTestCase{
                        "r7/p7/8/8/8/8/P7/R7 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("a8"), 0, 0, 0}
                        },
                        { "a8c8" }
                },
                // Rf1, Rh3
                ResponderTestCase{
                        "7r/p7/8/8/8/8/P7/7R w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("h1"), 0, 0, 0}
                        },
                        { "h1f1", "h1h3" }
                },
                // Rf8, Rh6
                ResponderTestCase{
                        "7r/8/8/8/8/8/8/7R b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("h8"), 0, 0, 0}
                        },
                        { "h8f8", "h8h6" }
                },
                // Qc2, Qd2, Qe2
                ResponderTestCase{
                        "3q4/8/8/8/8/8/8/3Q4 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d1"), 0, 0, 0}
                        },
                        { "d1c2", "d1d2", "d1e2" }
                },
                // Qc7, Qd7, Qe7
                ResponderTestCase{
                        "3q4/8/8/8/8/8/8/3Q4 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d8"), 0, 0, 0}
                        },
                        { "d8c7", "d8d7", "d8e7" }
                },

                // test that captures

                // test that chooses worse if blocked

                // medley

        }
};

bool evaluate_develop_test_case(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&develop_piece, tc);
}

void test_develop_piece() {
    evaluate_test_set(&develop_test_cases, &evaluate_develop_test_case);
}
