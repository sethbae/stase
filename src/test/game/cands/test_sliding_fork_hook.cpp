#include <string>
#include <vector>
#include <board.h>

#include "../../../game/cands/cands.h"
#include "../../test.h"

TestSet<StringTestCase> sliding_fork_hook_test_cases = {
        "game-cands-sliding-fork-hook",
        {
                StringTestCase{
                        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        {}
                },
                // BISHOP: basic undefended pieces #1
                StringTestCase{
                        "6r1/8/8/5B2/2r5/8/8/8 w - - 0 1",
                        {"e6"}
                },
                // BISHOP: basic undefended pieces #2
                StringTestCase{
                        "8/8/3b3N/8/8/8/8/2K5 b - - 0 1",
                        {"f4"}
                },
                // BISHOP: does not fork from unsafe square
                StringTestCase{
                        "8/8/3b3N/8/8/6P1/7P/2K5 b - - 0 1",
                        {}
                },
                // BISHOP: does not fork defended piece of lower/equal value
                StringTestCase{
                        "5r2/8/8/p7/1n6/4B3/8/8 w - - 0 1",
                        {}
                },
                // BISHOP: does not fork pieces which can move in that direction
                StringTestCase{
                        "8/8/4Q3/8/8/1K3b2/8/8 b - - 0 1",
                        {}
                },
//                // BISHOP: puzzle #1 TODO awaiting fork with capture
//                StringTestCase{
//                        "rbr2k2/ppn2ppp/8/2p5/2P2P2/P5P1/R3N1BP/2K4R w - - 0 22",
//                        {"b7"}
//                },
                // BISHOP: puzzle #2
                StringTestCase{
                        "r1b2rk1/pp3ppp/2p3n1/8/2P5/q3B3/4BQPP/1R3RK1 w - - 0 21",
                        {"c5"}
                },

                // ROOK: basic undefended pieces #1
                StringTestCase{
                        "4r3/8/8/1P4P1/8/8/8/8 w - - 0 1",
                        {"e5"}
                },
                // ROOK: basic undefended pieces #2
                StringTestCase{
                        "8/8/8/1b4n1/8/8/2R5/8 w - - 0 1",
                        {"c5"}
                },
                // ROOK: does not fork from unsafe square
                StringTestCase{
                        "8/8/1b6/1b4n1/8/8/2R5/8 w - - 0 1",
                        {}
                },
                // ROOK: does not fork defended piece of lower/equal value
                StringTestCase{
                        "8/8/3r4/8/8/2B3K1/1P6/8 b - - 0 1",
                        {}
                },
                // ROOK: does not fork pieces which can move in that direction
                StringTestCase{
                        "8/8/3r4/8/8/2R3K1/8/8 b - - 0 1",
                        {}
                },
//                // ROOK: puzzle #1  TODO waiting on bug in would_be_weak_if_attacked (GM-30)
//                StringTestCase{
//                        "4r2k/3q2p1/6Qp/p3p3/8/2P2R1P/6PK/8 w - - 2 37",
//                        {"f7"}
//                },
//                // ROOK: puzzle #2  TODO waiting on the bug in would_be_weak (GM-31)
//                StringTestCase{
//                        "8/8/3p1k2/3R4/5qP1/5B2/5PK1/8 w - - 3 54",
//                        {"f5"}
//                },
                // TODO this is failing because it tries to queen "fork" on d7
                // ROOK: puzzle #3
                StringTestCase{
                    "r4r1k/1pq1n1pp/2p1Q3/4p2P/4P3/1PP3P1/5P2/3R1RK1 w - - 1 26",
                    {"d7"}
                },

                // QUEEN: basic undefended pieces #1
                StringTestCase{
                        "",
                        {}
                },
                // QUEEN: basic undefended pieces #2
                StringTestCase{
                        "",
                        {}
                },
                // QUEEN: does not fork from unsafe square
                StringTestCase{
                        "3k4/2p5/5r2/2b1N3/8/8/8/8 w - - 0 1",
                        {}
                },
                // QUEEN: does not fork defended piece of lower/equal value
                StringTestCase{
                        "8/8/2n5/8/8/8/P1R5/2Q5 w - - 0 1",
                        {}
                },
                // QUEEN: does not fork pieces which can move in that direction
                StringTestCase{
                        "6q1/8/2n5/3N4/8/8/8/8 b - - 0 1",
                        {}
                },
                // QUEEN: can fork from several squares
                StringTestCase{
                        "6q1/8/2n5/3N4/8/8/8/8 b - - 0 1",
                        {}
                },
                // QUEEN: puzzle #1
                StringTestCase{
                        "5bk1/2p2p2/6pp/3N4/1r2q3/4PN1P/1P3PP1/5QK1 w - - 0 22",
                        {"f6"}
                },
                // QUEEN: puzzle #2
                StringTestCase{
                        "5r2/7p/kp1p1np1/3N4/2n5/8/P1r2PPP/3R1RK1 w - - 0 35",
                        {"b4"}
                },
        }
};

bool evaluate_test_case_sliding_fork_hook(const StringTestCase *tc) {

    Gamestate gs(fen_to_board(tc->fen));

    find_sliding_forks(gs, 0, nullptr);

    std::vector<std::string> strings;

    for (FeatureFrame* ff = gs.feature_frames[sliding_fork_hook.id]; ff->centre != SQUARE_SENTINEL; ++ff) {
        strings.push_back(sqtos(ff->centre));
    }

    // print_feature_frames(gs.feature_frames[0]);

    return assert_string_lists_equal(strings, tc->expected_results);
}

void test_sliding_fork_hook() {
    evaluate_test_set(&sliding_fork_hook_test_cases, &evaluate_test_case_sliding_fork_hook);
}
