#include <board.h>

#include "../../test.h"
#include "../../../game/gamestate.hpp"
#include "fork_helpers.h"

TestSet<ForkTestCase> queen_fork_hook_test_cases = {
    "game-cands-queen-fork-hook",
    {
        ForkTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {}
        },
        // does not find normal forks #1
        ForkTestCase{
            "8/6b1/3Q4/8/8/8/6n1/8 w - - 0 1",
            {}
        },
        // basic undefended pieces #1
        ForkTestCase{
            "1k6/3r4/7Q/8/8/8/6r1/K7 w - - 0 1",
            {
                ForkFrame{"d7", "g2", "c6", "h6"},
                ForkFrame{"d7", "g2", "h3", "h6"}
            }
        },
        // no forks reachable
        ForkTestCase{
            "8/8/3q4/8/4Q3/8/8/R5K1 b - - 0 1",
            {}
        },
        // does not find normal forks #2
        ForkTestCase{
            "8/1q6/8/8/8/8/N5B1/8 b - - 0 1",
            {}
        },
        // none available
        ForkTestCase{
            "8/4B3/3q4/8/8/8/8/4R3 b - - 0 1",
            {}
        },
        // does not fork defended piece of lower/equal value
        ForkTestCase{
            "8/4p3/3b4/8/8/8/1Q6/3n4 w - - 0 1",
            {}
        },
        // does not fork pieces which can move in that direction
        ForkTestCase{
            "8/8/8/8/8/3q4/1R4B1/8 b - - 0 1",
            {}
        },
        //  basic undefended pieces #2
        ForkTestCase{
            "7K/1p3p2/3Q4/8/8/8/8/k7 w - - 0 1",
            {
                ForkFrame{"b7", "f7", "d5", "d6"},
                ForkFrame{"a1", "f7", "f6", "d6"}
            }
        },
        //  gives multiple choices #1
        ForkTestCase{
            "8/8/3q4/8/8/8/1P3P2/8 b - - 0 1",
            {
                ForkFrame{"b2", "f2", "b6", "d6"},
                ForkFrame{"b2", "f2", "f6", "d6"},
                ForkFrame{"b2", "f2", "d4", "d6"}
            }
        },
        // does not fork defended piece of lower/equal value
        ForkTestCase{
            "8/6p1/5b2/8/2Q5/4n3/8/8 w - - 0 1",
            {}
        },
        // does not fork pieces which can move in that direction
        ForkTestCase{
            "8/3R4/8/2q5/8/3R4/8/8 b - - 0 1",
            {}
        },
        // forks are blocked by own piece!
        ForkTestCase{
            "1r3k2/8/3N4/8/8/3Q4/8/8 w - - 0 1",
            {}
        },
        // does not fork a piece which can just trade immediately
        ForkTestCase{
            "1q3r2/8/8/2B5/3Q4/8/8/8 w - - 0 1",
            {}
        },
        // puzzle #1
        ForkTestCase{
            "r2q4/5ppk/p2p3p/1p1P4/2p1rNQ1/8/PPP3PP/5RK1 w - - 0 23",
            {
                ForkFrame{"f7", "e4", "f5", "g4"}
            }
        },
        // puzzle #2
        ForkTestCase{
            "r1b1k2r/ppq2ppp/5n2/3p4/1b1P4/2NBB3/PP3PPP/R2QK2R w KQkq - 3 11",
            {
                ForkFrame{"e8", "b4", "a4", "d1"},
            }
        },
        // puzzle #3 (the correct solution is not actually a fork, but this tests the logic regardless)
        ForkTestCase{
            "r5qr/p1p1p2p/2Qp2p1/5k2/5n2/5P2/PP3K1P/2R3NR w - - 0 21",
            {
                ForkFrame{"f5", "e7", "e4", "c6"},
            }
        }
    }
};

bool evaluate_test_case_queen_fork_hook(const ForkTestCase *tc) {
    return evaluate_fork_test_case(tc, QUEEN);
}

bool test_queen_fork_hook() {
     return evaluate_test_set(&queen_fork_hook_test_cases, &evaluate_test_case_queen_fork_hook);
}
