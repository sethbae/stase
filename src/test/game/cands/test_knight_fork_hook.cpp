#include <string>
#include <vector>
#include <board.h>

#include "../../../game/cands/cands.h"
#include "../../../game/gamestate.hpp"
#include "../../test.h"

struct ForkFrame {
    const std::string forked_a;
    const std::string forked_b;
    const std::string fork_sq;
    const std::string from_sq;
};

struct ForkTestCase {
    const std::string fen;
    const std::vector<ForkFrame> expected_frames;
};

TestSet<ForkTestCase> knight_fork_hook_test_cases = {
        "game-cands-knight-fork-hook",
        {
            ForkTestCase{
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                {}
            },
            // basic undefended pieces #1
            ForkTestCase{
                "8/2p3p1/8/8/3N4/8/8/8 b - - 0 1",
                {
                    ForkFrame{"c7", "g7", "e6", "d4"}
                }
            },
            // basic undefended pieces #2
            ForkTestCase{
                "8/8/8/8/4R1n1/3R4/8/8 w - - 0 1",
                {
                    ForkFrame{"d3", "e4", "f2", "g4"}
                }
            },
            // basic undefended pieces #3
            ForkTestCase{
                "8/b1p5/8/8/8/2N5/8/8 b - - 0 1",
                {
                    ForkFrame{"a7", "c7", "b5", "c3"}
                }
            },
            // does not fork from unsafe squares #1
            ForkTestCase{
                "3k4/2p5/5r2/2b1N3/8/8/8/8 w - - 0 1",
                {}
            },
            // does not fork from unsafe squares #2
            ForkTestCase{
                "8/8/8/8/8/8/3n4/4Q1R1 b - - 0 1",
                {
                    ForkFrame{"e1", "g1", "f3", "d2"}
                }
            },
            // does not fork defended piece of lower value #1
            ForkTestCase{
                "8/8/2n5/8/8/8/P1R5/2Q5 w - - 0 1",
                {}
            },
            // does not fork defended piece of lower value #2
            ForkTestCase{
                "6q1/1p6/2b5/3N4/8/8/8/8 w - - 0 1",
                {}
            },
            // does not fork knights #1
            ForkTestCase{
                "6q1/8/2n5/3N4/8/8/8/8 b - - 0 1",
                {}
            },
            // does not fork knights #2
            ForkTestCase{
                "8/8/8/8/2n5/8/8/1N3K2 b - - 0 1",
                {}
            },
            // forks with capture #1
            ForkTestCase{
                "8/3b4/6q1/4p3/8/3N4/8/8 w - - 0 1",
                {
                    ForkFrame{"d7", "g6", "e5", "d3"}
                }
            },
            // doesn't fork pieces already attacked
            ForkTestCase{
                "8/8/5B2/3n4/8/2R5/8/8 b - - 0 1",
                {}
            },
            // fork is blocked by piece of own colour
            ForkTestCase{
                "8/8/8/8/1Q1n4/8/2b5/4K3 w - - 0 1",
                {}
            },
            // triple fork!
            ForkTestCase{
                "2q3r1/8/8/3N1k2/8/8/8/5K2 w - - 0 1",
                {
                    ForkFrame{"g8", "c8", "e7", "d5"}
                }
            },
            // puzzle #1
            ForkTestCase{
                "5bk1/2p2p2/6pp/3N4/1r2q3/4PN1P/1P3PP1/5QK1 w - - 0 22",
                {
                    ForkFrame{"g8", "e4", "f6", "d5"}
                }
            },
            // puzzle #2
            ForkTestCase{
                "5r2/7p/kp1p1np1/3N4/2n5/8/P1r2PPP/3R1RK1 w - - 0 35",
                {
                    ForkFrame{"a6", "c2", "b4", "d5"}
                }
            },
            // puzzle #3
            ForkTestCase{
                "r6k/p2n1prp/3q3N/2pppp2/8/1PQ4P/P1P2PP1/R3R1K1 w - - 0 21",
                {
                    ForkFrame{"d6", "g7", "f5", "h6"}
                }
            },
            // puzzle #4
            ForkTestCase{
                "r2qkbnr/p4ppp/2p1p3/1N6/Q1pP1B2/4PN2/PP1K1PPP/2n4R w kq - 0 12",
                {
                    ForkFrame{"a8", "e8", "c7", "b5"},
                    ForkFrame{"b2", "f2", "d3", "c1"},
                    ForkFrame{"c6", "c4", "e5", "f3"},
                }
            },
            ForkTestCase{
                "r1b1kbnr/1ppp1ppp/p1n2q2/4p2Q/2B1P3/2N5/PPPP1PPP/R1B1K1NR w KQkq - 0 5",
                {
                    ForkFrame{"c7", "f6", "d5", "c3"}
                }
            }
        }
};

bool fork_frames_equivalent(const ForkFrame & expected, const FeatureFrame & actual) {

    if ((expected.forked_a == sqtos(actual.centre) && expected.forked_b == sqtos(actual.secondary))
        || (expected.forked_b == sqtos(actual.centre) && expected.forked_a == sqtos(actual.secondary))) {
        if (sqtos(itosq(actual.conf_1)) == expected.fork_sq
            && sqtos(itosq(actual.conf_2)) == expected.from_sq) {
            return true;
        }
    }
    return false;
}

bool list_contains_match(const std::vector<ForkFrame> expecteds, const FeatureFrame & actual) {
    for (const ForkFrame & expected : expecteds) {
        if (fork_frames_equivalent(expected, actual)) {
            return true;
        }
    }
    return false;
}

bool evaluate_test_case_knight_fork_hook(const ForkTestCase * tc) {

    Gamestate gs(tc->fen);

    discover_feature_frames(gs, &fork_hook);

    std::vector<std::string> strings;

    int actual_frames_encountered = 0;

    for (int i = 0; !is_sentinel(gs.frames[fork_hook.id][i].centre) && i < MAX_FRAMES; ++i) {

        FeatureFrame ff = gs.frames[fork_hook.id][i];
        if (ff.conf_2 == sq_sentinel_as_int()) {
            continue;
        }
        if (type(gs.board.get(itosq(ff.conf_2))) != KNIGHT) {
            continue;
        }

        ++actual_frames_encountered;
        if (!list_contains_match(tc->expected_frames, ff)) {
            return false;
        }
    }

    return tc->expected_frames.size() == actual_frames_encountered;
}

bool test_knight_fork_hook() {
    return evaluate_test_set(&knight_fork_hook_test_cases, &evaluate_test_case_knight_fork_hook);
}
