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
        return "game-cands-weak";
    }

    const std::string fen;
    const std::vector<std::string> squares;
};

const std::vector<TestCase> test_cases{
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

int count_to_sentinel(const FeatureFrame *ff) {
    int n = 0;

    for (const FeatureFrame *ptr = ff; ptr->centre != SQUARE_SENTINEL; ptr++) {
        ++n;
    }

    return n;
}

FeatureFrame *create_frame_list(const std::vector<std::string> strings) {

    FeatureFrame *frames = new FeatureFrame[strings.size() + 1];
    int i = 0;

    for (std::string s : strings) {
        frames[i].centre = stosq(s);
        frames[i++].secondary = 0;
    }

    frames[i].centre = SQUARE_SENTINEL;
    frames[i].secondary = SQUARE_SENTINEL;

    return frames;

}

bool assert_frame_lists_equal(const FeatureFrame *ff_a, const FeatureFrame *ff_b) {

    int a_n = count_to_sentinel(ff_a), b_n = count_to_sentinel(ff_b);

    if (a_n != b_n) { return false; }

    for (const FeatureFrame *a = ff_a; a->centre != SQUARE_SENTINEL; ++a) {
        bool present = false;
        for (const FeatureFrame *b = ff_b; b->centre != SQUARE_SENTINEL; ++b) {
            if (a->centre == b->centre && a->secondary == b->secondary) {
                present = true;
                break;
            }
        }
        if (!present) {
            return false;
        }
    }

    return true;

}

bool evaluate_test_case(const TestCase *tc) {

    Gamestate gs(fen_to_board(tc->fen));

    weak_hook(gs.board, &gs.feature_frames[0]);

    // print_feature_frames(gs.feature_frames[0]);

    return assert_frame_lists_equal(gs.feature_frames[0], create_frame_list(tc->squares));
}

void test_weak() {
    evaluate_test_set(test_cases, &evaluate_test_case);
}
