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
                FeatureFrame{stosq("f1"), SQUARE_SENTINEL, 0, 0}
            },
            { "f1e2", "f1g2" }
        },
        // Be7, Bg7
        ResponderTestCase{
            "5b2/8/8/8/8/8/8/5B2 b - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("f8"), SQUARE_SENTINEL, 0, 0}
            },
            { "f8e7", "f8g7" }
        },
        // Nf3
        ResponderTestCase{
            "6n1/8/8/8/8/8/8/6N1 w - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("g1"), SQUARE_SENTINEL, 0, 0}
            },
            { "g1f3" }
        },
        // Nf6
        ResponderTestCase{
            "6n1/8/8/8/8/8/8/6N1 b - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("g8"), SQUARE_SENTINEL, 0, 0}
            },
            { "g8f6" }
        },
        // Rc1
        ResponderTestCase{
            "r7/p7/8/8/8/8/P7/R7 w - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("a1"), SQUARE_SENTINEL, 0, 0}
            },
            { "a1c1" }
        },
        // Rc8
        ResponderTestCase{
            "r7/p7/8/8/8/8/P7/R7 b - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("a8"), SQUARE_SENTINEL, 0, 0}
            },
            { "a8c8" }
        },
        // Rf1, Rh3
        ResponderTestCase{
            "7r/p7/8/8/8/8/P7/7R w - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("h1"), SQUARE_SENTINEL, 0, 0}
            },
            { "h1f1", "h1h3" }
        },
        // Rf8, Rh6
        ResponderTestCase{
            "7r/8/8/8/8/8/8/7R b - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("h8"), SQUARE_SENTINEL, 0, 0}
            },
            { "h8f8", "h8h6" }
        },
        // Qc2, Qd2, Qe2
        ResponderTestCase{
            "3q4/8/8/8/8/8/8/3Q4 w - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("d1"), SQUARE_SENTINEL, 0, 0}
            },
            { "d1c2", "d1d2", "d1e2" }
        },
        // Qc7, Qd7, Qe7
        ResponderTestCase{
            "3q4/8/8/8/8/8/8/3Q4 b - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("d8"), SQUARE_SENTINEL, 0, 0}
            },
            { "d8c7", "d8d7", "d8e7" }
        },
        // test that captures
        ResponderTestCase{
            "6n1/8/8/8/8/8/8/6N1 w - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("g1"), SQUARE_SENTINEL, 0, 0}
            },
            { "g1f3" }
        },
        // test that chooses worse if blocked
        ResponderTestCase{
            "6n1/8/5n2/8/8/8/8/8 b - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("g8"), SQUARE_SENTINEL, 0, 0}
            },
            { "g8e7" }
        },
        // castles instead of playing Rf1
        ResponderTestCase{
            "r3k2r/p1pppppp/1p6/8/8/5B2/PPPPPPPP/R3K2R w KQkq - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("e1"), SQUARE_SENTINEL, 0, 0}
            },
            { "e1g1", "e1c1" }
        },
        // castles instead of playing Rf8, and only on the side that's legal
        ResponderTestCase{
            "r3k2r/pBpppppp/1p6/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("e8"), SQUARE_SENTINEL, 0, 0}
            },
            { "e8g8" }
        },
        // moves white pawn 2 squares or 1
        ResponderTestCase{
          "8/8/6pp/8/8/8/PPPPPPPP/8 w - - 0 1",
          std::vector<FeatureFrame>{
              FeatureFrame{stosq("f2"), SQUARE_SENTINEL, 0, 0}
          },
          { "f2f3", "f2f4" }
        },
        // moves black pawn only one
        ResponderTestCase{
            "8/8/6pp/8/8/8/PPPPPPPP/8 b - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("g6"), SQUARE_SENTINEL, 0, 0}
            },
            { "g6g5" }
        },
        // does not make a single pawn move to an unsupported square
        ResponderTestCase{
            "8/8/4p3/8/8/8/8/8 w - - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("d6"), SQUARE_SENTINEL, 0, 0}
            },
            { }
        },
        // medley # 1
        ResponderTestCase{
            "rnbqkbnr/ppp2ppp/3p4/3Pp3/4P3/5N2/PPP2PPP/RNBQKB1R w KQkq - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("a1"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("b1"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("c1"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("d1"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("e1"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("f1"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("g1"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("h1"), SQUARE_SENTINEL, 0, 0},
            },
            { "b1c3", "f1e2", "c1d2", "d1d2", "d1e2" }
        },
        // medley # 2
        ResponderTestCase{
            "rnbqkb1r/ppp1pp1p/3p1np1/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
            std::vector<FeatureFrame>{
                FeatureFrame{stosq("a8"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("b8"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("c8"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("d8"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("e8"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("f8"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("g8"), SQUARE_SENTINEL, 0, 0},
                FeatureFrame{stosq("h8"), SQUARE_SENTINEL, 0, 0},
            },
            { "b8c6", "c8d7", "d8d7", "f8g7" }
        }
    }
};

bool evaluate_develop_test_case(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&develop_resp, tc);
}

bool test_develop_piece() {
    return evaluate_test_set(&develop_test_cases, &evaluate_develop_test_case);
}
