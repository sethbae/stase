#include <string>
#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../test.h"
#include "../../../game/heur/heur.h"
#include "../../../game/helper.h"

struct TestCase {

    std::string name() const {
        return "game-cands-weak-hook";
    }

    const std::string fen;
    const std::vector<std::string> squares;
};

const std::vector<TestCase> hook_test_cases{
        // starting pos
        TestCase{
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                {},
        },
        // hanging piece pairs
        TestCase{  // bishop
            "B7/8/8/8/8/8/8/7b w - - 0 1",
            {"a8", "h1"}
        },
        TestCase{  // knight
            "8/8/5N2/3n4/8/8/8/8 w - - 0 1",
            {"d5", "f6"}
        },
        TestCase{  // queen
        "q7/8/8/8/8/8/8/7Q w - - 0 1",
        {"a8", "h1"}
        },
        TestCase{  // rook
            "2R5/8/8/8/8/8/8/2r5 w - - 0 1",
        {"c8", "c1"}
        },
        TestCase{  // pawn
            "8/8/8/8/8/p7/1P6/8 w - - 0 1",
        {"a3", "b2"}
        },
        // king takes piece
        TestCase{  // white king
            "8/8/8/8/8/1r6/K7/8 w - - 0 1",
            {"b3"}
        },
        TestCase{  // black king
            "8/1k6/1Q6/8/8/8/8/8 w - - 0 1",
        {"b6", "b7"}
        },
        // doubled rooks facing each other
        TestCase{
                "4r2b/4r3/8/3p4/5P2/8/4R3/B3R3 w - - 0 1",
                {"e2", "e7"},
        },
        // doubled rooks plus bishop in corner
        TestCase{
            "7b/4r3/8/3p4/5P2/8/4R3/B3R3 w - - 0 1",
                {"e7", "h8"},
        },
        // queen knight medley
        TestCase{
                "1q6/k1N5/8/2KQ4/8/8/8/8 w - - 0 1",
                {"c7"},
        },
        // queen knight medley #2
        TestCase{
            "8/k1N5/q7/2KQ4/8/8/8/8 w - - 0 1",
                {"a6"},
        },
        // hanging knight
        TestCase{
                "8/k1N2n2/8/2KQ4/4B3/5P2/6P1/8 w - - 0 1",
                {"f7"},
        },
        // pawn threatens knight
        TestCase{
            "8/8/8/3p4/4N3/8/8/8 w - - 0 1",
                {"e4"},
        },
        // does not take defended piece of same value
        TestCase{
            "6b1/8/8/3N4/2P5/8/8/8 w - - 0 1",
            {}
        },
        // does take defended piece of higher value
        TestCase{
            "6b1/8/8/3Q4/2P5/8/8/8 w - - 0 1",
            {"d5", "g8"}
        }
};

bool evaluate_test_case_weak_hook(const TestCase *tc) {

    Gamestate gs(fen_to_board(tc->fen));

    weak_hook(gs.board, &gs.feature_frames[0]);

    std::vector<std::string> strings;

    for (FeatureFrame* ff = gs.feature_frames[0]; ff->centre != SQUARE_SENTINEL; ++ff) {
        strings.push_back(sqtos(ff->centre));
    }

    // print_feature_frames(gs.feature_frames[0]);

    return assert_string_lists_equal(strings, tc->squares);
}

void test_weak_hook() {
    evaluate_test_set(hook_test_cases, &evaluate_test_case_weak_hook);
}