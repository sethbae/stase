#include <string>
#include <vector>
#include <board.h>

#include "../../../game/cands/cands.h"
#include "../../test.h"

// TODO: test capturing, test being blocked by your own piece

TestSet<StringTestCase> knight_fork_hook_test_cases = {
        "game-cands-knight-fork-hook",
        {
            StringTestCase{
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                {}
            },
            // basic undefended pieces #1
            StringTestCase{
                "8/2p3p1/8/8/3N4/8/8/8 b - - 0 1",
                {"e6"}
            },
            // basic undefended pieces #2
            StringTestCase{
                "8/8/8/8/4R1n1/3R4/8/8 w - - 0 1",
                {"f2"}
            },
            // basic undefended pieces #3
            StringTestCase{
                "8/b1p5/8/8/8/2N5/8/8 b - - 0 1",
                {"b5"}
            },
            // does not fork from unsafe squares #1
            StringTestCase{
                "3k4/2p5/5r2/2b1N3/8/8/8/8 w - - 0 1",
                {}
            },
            // does not fork from unsafe squares #2
            StringTestCase{
                "8/8/8/8/8/8/3n4/4Q1R1 b - - 0 1",
                {"f3"}
            },
            // does not fork defended piece of lower value #1
            StringTestCase{
                "8/8/2n5/8/8/8/P1R5/2Q5 w - - 0 1",
                {}
            },
            // does not fork defended piece of lower value #2
            StringTestCase{
                "6q1/1p6/2b5/3N4/8/8/8/8 w - - 0 1",
                {}
            },
            // does not fork knights #1
            StringTestCase{
                "6q1/8/2n5/3N4/8/8/8/8 b - - 0 1",
                {}
            },
            // does not fork knights #2
            StringTestCase{
                "8/8/8/8/2n5/8/8/1N3K2 b - - 0 1",
                {}
            },
            // triple fork!
            StringTestCase{
                "2q3r1/8/8/3N1k2/8/8/8/5K2 w - - 0 1",
                {"e7"}
            },
            // puzzle #1
            StringTestCase{
                "5bk1/2p2p2/6pp/3N4/1r2q3/4PN1P/1P3PP1/5QK1 w - - 0 22",
                {"f6"}
            },
            // puzzle #2
            StringTestCase{
                "5r2/7p/kp1p1np1/3N4/2n5/8/P1r2PPP/3R1RK1 w - - 0 35",
                {"b4"}
            },
            // puzzle #3
            StringTestCase{
                "r6k/p2n1prp/3q3N/2pppp2/8/1PQ4P/P1P2PP1/R3R1K1 w - - 0 21",
                {"f5"}
            },
            // puzzle #4
            StringTestCase{
                "r2qkbnr/p4ppp/2p1p3/1N6/Q1pP1B2/4PN2/PP1K1PPP/2n4R w kq - 0 12",
                {"c7", "d3", "e5"}
            },
            StringTestCase{
                "r1b1kbnr/1ppp1ppp/p1n2q2/4p2Q/2B1P3/2N5/PPPP1PPP/R1B1K1NR w KQkq - 0 5",
                {"d5"}
            }
        }
};

bool evaluate_test_case_knight_fork_hook(const StringTestCase *tc) {

    Gamestate gs(fen_to_board(tc->fen));

    discover_feature_frames(gs, &fork_hook);

    std::vector<std::string> strings;

    for (FeatureFrame* ff = gs.feature_frames[fork_hook.id]; ff->centre != SQUARE_SENTINEL; ++ff) {
        // only look at knight forks for these tests!
        if (type(gs.board.get(ff->secondary)) == KNIGHT) {
            strings.push_back(sqtos(ff->centre));
        }
    }

    // print_feature_frames(gs.feature_frames[0]);

    return assert_string_lists_equal(strings, tc->expected_results);
}

void test_knight_fork_hook() {
    evaluate_test_set(&knight_fork_hook_test_cases, &evaluate_test_case_knight_fork_hook);
}
