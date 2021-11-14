#include <string>
#include <vector>
#include <board.h>

#include "../../../game/cands/cands.h"
#include "../../test.h"

TestSet<StringTestCase> kp_fork_hook_test_cases = {
        "game-cands-kp-fork-hook",
        {
                StringTestCase{
                        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        {}
                },
                // PAWN: basic undefended pieces #1
                StringTestCase{
                        "r1b1kbnr/pppppppp/8/8/3n1q2/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        {"e3"}
                },
                // PAWN: basic undefended pieces #2
                StringTestCase{
                        "rnbqkb1r/pppp1ppp/8/2B1N3/8/8/PPPP1PPP/R1BQK1NR b KQkq - 0 1",
                        {"d6"}
                },
                // PAWN: does not fork from unsafe square
                StringTestCase{
                        "8/n1n5/7r/1P6/8/8/8/8 w - - 0 1",
                        {}
                },
                // PAWN: does not fork other pawns
                StringTestCase{
                        "8/8/3p4/8/2P1N3/8/8/8 b - - 0 1",
                        {}
                },
                // PAWN: forks with capture
                StringTestCase{
                        "8/8/2r1n3/3p4/2P5/8/8/8 w - - 0 1",
                        {"d5"}
                },
                // PAWN: doesn't fork pieces already attacked
                StringTestCase{
                        "8/8/8/3p4/2R1N3/8/8/8 b - - 0 1",
                        {}
                },
                // PAWN: fork is blocked by own piece
                StringTestCase{
                        "8/8/1r1r4/2N5/2P5/8/8/8 w - - 0 1",
                        {}
                },
                // PAWN: forks with double move #1
                StringTestCase{
                        "8/8/8/r1b5/8/2P5/1P6/8 w - - 0 1",
                        {"b4"}
                },
                // PAWN: forks with double move #2
                StringTestCase{
                        "8/5p2/4p3/8/4B1N1/8/8/8 w - - 0 1",
                        {"f5"}
                },
                // PAWN: forks with ep #1
                StringTestCase{
                        "8/2q1k3/4p3/2PpP3/8/8/8/8 w - d6 0 1",
                        {"d6", "d6"}
                },
                // PAWN: forks with ep #2
                StringTestCase{
                        "8/8/1b6/8/4Pp2/8/3R1B2/8 w - e3 0 1",
                        {"e3"}
                },
                // PAWN: puzzle #1
                StringTestCase{
                        "2r5/4k1r1/p2p4/1p1PnPPR/1Pp1P3/P1R3K1/8/8 w - - 0 43",
                        {"f6"}
                },
                // PAWN: puzzle #2
                StringTestCase{
                        "8/8/1rB2p2/1P2k3/2PbN1K1/7P/6P1/8 b - - 2 58",
                        {"f5"}
                },

                // KING: basic undefended pieces #1
                StringTestCase{
                        "8/8/8/4bn2/8/4K3/8/8 w - - 0 1",
                        {"e4"}
                },
                // KING: basic undefended pieces #2
                StringTestCase{
                        "8/4P3/2k1N3/8/8/8/8/8 b - - 0 1",
                        {"d7", "d6"}
                },
                // KING: does not fork from unsafe square #1
                StringTestCase{
                        "8/3pbn2/8/3K4/8/8/8/8 w - - 0 1",
                        {}
                },
                // KING: does not fork from unsafe square #2
                StringTestCase{
                        "8/8/8/4k3/8/5NN1/3Q4/8 b - - 0 1",
                        {}
                },
                // KING: does not fork defended pieces
                StringTestCase{
                        "8/8/2rr4/8/3K4/8/8/8 w - - 0 1",
                        {}
                },
                // KING: forks with capture
                StringTestCase{
                        "8/8/8/6k1/6P1/5NB1/8/8 b - - 0 1",
                        {"g4"}
                },
                // KING: doesn't fork pieces already attacked
                StringTestCase{
                        "8/8/8/3nn3/3K4/8/8/8 w - - 0 1",
                        {}
                }
        }
};

bool evaluate_test_case_kp_fork_hook(const StringTestCase *tc) {

    Gamestate gs(fen_to_board(tc->fen));

    discover_feature_frames(gs, &fork_hook);

    std::vector<std::string> strings;

    for (FeatureFrame* ff = gs.feature_frames[fork_hook.id]; ff->centre != SQUARE_SENTINEL; ++ff) {
        // only look at king/pawn forks for these tests!
        if (type(gs.board.get(ff->centre)) == KING || type(gs.board.get(ff->centre)) == PAWN) {
            strings.push_back(sqtos(ff->secondary));
        }
    }

    return assert_string_lists_equal(strings, tc->expected_results);
}

bool test_kp_fork_hook() {
    return evaluate_test_set(&kp_fork_hook_test_cases, &evaluate_test_case_kp_fork_hook);
}
