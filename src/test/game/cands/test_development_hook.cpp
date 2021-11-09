#include "board.h"
#include "game.h"
#include "../../test.h"

#include <vector>

const TestSet<StringTestCase> hook_test_set{
        "game-cands-development-hook",
        {
                // rooks do
                StringTestCase{
                        "r6r/8/8/8/8/8/8/R6R w - - 0 1",
                        {"a1", "a8", "h1", "h8"}
                },
                // queens do
                StringTestCase{
                        "3q4/8/8/8/8/8/8/3Q4 b - - 0 1",
                        {"d1", "d8"}
                },
                // bishops do
                StringTestCase{
                        "2b2b2/8/8/8/8/8/8/2B2B2 w - - 0 1",
                        {"c1", "f1", "c8", "f8"}
                },
                // knights do
                StringTestCase{
                        "1n4n1/8/8/8/8/8/8/1N4N1 b - - 0 1",
                        {"b1", "g1", "b8", "g8"}
                },
                // kings do
                StringTestCase{
                        "4k3/8/8/8/8/8/8/4K3 w - - 0 1",
                        {"e1", "e8"}
                },
                // pawns do
                StringTestCase{
                        "8/8/8/1p6/8/8/6P1/8 w - - 0 1",
                        {"g2"}
                },
                // pawns don't
                StringTestCase{
                        "8/8/8/1p6/8/8/8/8 b - - 0 1",
                        {}
                },
                // others don't #1
                StringTestCase{
                    "bkq1r1bn/4r3/5Q2/8/8/1PP1N1P1/R3B3/1R1K2PP b - - 0 1",
                    {}
                },
                // others don't #2
                StringTestCase{
                        "8/5k2/p1b5/1p5p/1Pr4P/4R1P1/1R3K2/8 b - - 6 38",
                        {}
                }
        }

};

bool evaluate_test_case_development_hook(const StringTestCase *tc) {

    Gamestate gs(fen_to_board(tc->fen));

    discover_feature_frames(gs, &develop_hook);

    std::vector<std::string> strings;

    for (FeatureFrame *ff = gs.feature_frames[develop_hook.id]; ff->centre != SQUARE_SENTINEL; ++ff) {
        strings.push_back(sqtos(ff->centre));
    }

    // print_feature_frames(gs.feature_frames[0]);

    return assert_string_lists_equal(strings, tc->expected_results);
}

void test_development_hook() {
    evaluate_test_set(&hook_test_set, &evaluate_test_case_development_hook);
}
