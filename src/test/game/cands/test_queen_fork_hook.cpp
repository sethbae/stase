#include <string>
#include <vector>
#include <board.h>

#include "../../../game/cands/cands.h"
#include "../../test.h"

TestSet<StringTestCase> queen_fork_hook_test_cases = {
        "game-cands-queen-fork-hook",
        {
                StringTestCase{
                        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        {}
                },
                // (STRAIGHT) basic undefended pieces #1
                StringTestCase{
                        "8/6b1/3Q4/8/8/8/6n1/8 w - - 0 1",
                        {"g6", "g3"}
                },
                // (STRAIGHT) does not fork from unsafe square
                StringTestCase{
                        "8/4B3/3q4/8/8/8/8/4R3 b - - 0 1",
                        {}
                },
                // (STRAIGHT) does not fork defended piece of lower/equal value
                StringTestCase{
                        "8/4p3/3b4/8/8/8/1Q6/3n4 w - - 0 1",
                        {}
                },
                // (STRAIGHT) does not fork pieces which can move in that direction
                StringTestCase{
                        "8/8/8/8/8/3q4/1R4B1/8 b - - 0 1",
                        {}
                },
                // (STRAIGHT) gives multiple choices #1
                StringTestCase{
                        "8/1p3p2/3Q4/8/8/8/8/8 w - - 0 1",
                        {"c7", "d7", "e7", "d5"}
                },
                // (STRAIGHT) gives multiple choices #2
                StringTestCase{
                        "8/8/3q4/8/8/8/1P3P2/8 b - - 0 1",
                        {"b6", "f6", "d4", "d2"}
                },
                // (MIXED) basic undefended pieces #1
                StringTestCase{
                        "8/3r4/7Q/8/8/8/6r1/8 w - - 0 1",
                        {"c6", "h3"}
                },
                // (MIXED) does not fork from unsafe square
                StringTestCase{
                        "8/8/3q4/8/4Q3/8/8/R5K1 b - - 0 1",
                        {}
                },
                // (MIXED) does not fork defended piece of lower/equal value
                StringTestCase{
                        "8/6p1/5b2/8/2Q5/4n3/8/8 w - - 0 1",
                        {}
                },
                // (MIXED) does not fork pieces which can move in that direction
                StringTestCase{
                        "8/3R4/8/2q5/8/3R4/8/8 b - - 0 1",
                        {}
                },
                // forks with a capture #1
                StringTestCase{
                        "r3kbnr/p3pppp/n1p5/8/2P5/1P2PQ2/PB3PPP/RN3RK1 w - - 0 1",
                        {"c6"}
                },
                // forks with a capture #2
                StringTestCase{
                        "rnbqkbnr/pp2pppp/2pp4/P5B1/8/N2P4/1PP1PPPP/R2QKBNR b KQkq - 0 1",
                        {"a5"}
                },
                // puzzle #1
                StringTestCase{
                        "r2q4/5ppk/p2p3p/1p1P4/2p1rNQ1/8/PPP3PP/5RK1 w - - 0 23",
                        {"f5"}
                },
                // puzzle #2
                StringTestCase{
                        "r1b1k2r/ppq2ppp/5n2/3p4/1b1P4/2NBB3/PP3PPP/R2QK2R w KQkq - 3 11",
                        {"a4"}
                },
                // puzzle #3
                StringTestCase{
                        "1k5n/pbp2q2/1p2p3/3p1pQ1/3P2p1/2P1P1P1/PPBN2P1/2K5 w - - 0 29",
                        {"d8"}
                },
                // puzzle #4 (the correct solution is not actually a fork, but this tests the logic regardless.
                // there are two minor pawn forks it finds!)
                StringTestCase{
                        "r5qr/p1p1p2p/2Qp2p1/5k2/5n2/5P2/PP3K1P/2R3NR w - - 0 21",
                        {"e4", "d7"}
                }
        }
};

bool evaluate_test_case_queen_fork_hook(const StringTestCase *tc) {

    Gamestate gs(fen_to_board(tc->fen));

    find_queen_forks(gs, 0, nullptr);

    std::vector<std::string> strings;

    for (FeatureFrame* ff = gs.feature_frames[queen_fork_hook.id]; ff->centre != SQUARE_SENTINEL; ++ff) {
        strings.push_back(sqtos(ff->centre));
    }

    // print_feature_frames(gs.feature_frames[0]);

    return assert_string_lists_equal(strings, tc->expected_results);
}

void test_queen_fork_hook() {
    evaluate_test_set(&queen_fork_hook_test_cases, &evaluate_test_case_queen_fork_hook);
}
