#include <string>
#include <vector>
#include <board.h>

#include "../../../game/cands/cands.h"
#include "../../../game/gamestate.hpp"
#include "../../test.h"
#include "fork_helpers.h"

TestSet<ForkTestCase> pawn_fork_hook_test_cases = {
    "game-cands-pawn-fork-hook",
    {
        ForkTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {}
        },
        // PAWN: basic undefended pieces #1
        ForkTestCase{
            "r1b1kbnr/pppppppp/8/8/3n1q2/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {
                ForkFrame{"d4", "f4", "e3", "e2"}
            }
        },
        // PAWN: basic undefended pieces #2
        ForkTestCase{
            "rnbqkb1r/pppp1ppp/8/2B1N3/8/8/PPPP1PPP/R1BQK1NR b KQkq - 0 1",
            {
                ForkFrame{"c5", "e5", "d6", "d7"}
            }
        },
        // PAWN: does not fork from unsafe square
        ForkTestCase{
            "8/n1n5/7r/1P6/8/8/8/8 w - - 0 1",
            {}
        },
        // PAWN: does not fork other pawns
        ForkTestCase{
            "8/8/3p4/8/2P1N3/8/8/8 b - - 0 1",
            {}
        },
        // PAWN: forks with capture
        ForkTestCase{
            "8/8/2r1n3/3p4/2P5/8/8/8 w - - 0 1",
            {
                ForkFrame{"c6", "e6", "d5", "c4"}
            }
        },
        // PAWN: doesn't fork pieces already attacked
        ForkTestCase{
            "8/8/8/3p4/2R1N3/8/8/8 b - - 0 1",
            {}
        },
        // PAWN: fork is blocked by own piece
        ForkTestCase{
            "8/8/1r1r4/2N5/2P5/8/8/8 w - - 0 1",
            {}
        },
        // PAWN: forks with double move #1
        ForkTestCase{
            "8/8/8/r1b5/8/2P5/1P6/8 w - - 0 1",
            {
                ForkFrame{"a5", "c5", "b4", "b2"}
            }
        },
        // PAWN: forks with double move #2
        ForkTestCase{
            "8/5p2/4p3/8/4B1N1/8/8/8 w - - 0 1",
            {
                ForkFrame{"e4", "g4", "f5", "f7"}
            }
        },
        // PAWN: forks with ep #1
        ForkTestCase{
            "8/2q1k3/4p3/2PpP3/8/8/8/8 w - d6 0 1",
            {
                ForkFrame{"c7", "e7", "d6", "c5"},
                ForkFrame{"c7", "e7", "d6", "e5"}
            }
        },
        // PAWN: forks with ep #2
        ForkTestCase{
            "8/8/1b6/8/4Pp2/8/3R1B2/8 b - e3 0 1",
            {
                ForkFrame{"d2", "f2", "e3", "f4"}
            }
        },
        // PAWN: puzzle #1
        ForkTestCase{
            "2r5/4k1r1/p2p4/1p1PnPPR/1Pp1P3/P1R3K1/8/8 w - - 0 43",
            {
                ForkFrame{"e7", "g7", "f6", "f5"}
            }
        },
        // PAWN: puzzle #2
        ForkTestCase{
            "8/8/1rB2p2/1P2k3/2PbN1K1/7P/6P1/8 b - - 2 58",
            {
                ForkFrame{"e4", "g4", "f5", "f6"}
            }
        },
    }
};

const TestSet<ForkTestCase> king_fork_hook_test_cases{
    "game-cands-king-fork-hook",
    {
        // KING: basic undefended pieces #1
        ForkTestCase{
            "8/8/8/4bn2/8/4K3/8/8 w - - 0 1",
            {
                ForkFrame{"e5", "f5", "e4", "e3"}
            }
        },
        // KING: basic undefended pieces #2
        ForkTestCase{
            "8/4P3/2k1N3/8/8/8/8/8 b - - 0 1",
            {
                ForkFrame{"e7", "e6", "d6", "c6"},
                ForkFrame{"e7", "e6", "d7", "c6"}
            }
        },
        // KING: does not fork from unsafe square #1
        ForkTestCase{
            "8/3pbn2/8/3K4/8/8/8/8 w - - 0 1",
            {}
        },
        // KING: does not fork from unsafe square #2
        ForkTestCase{
            "8/8/8/4k3/8/5NN1/3Q4/8 b - - 0 1",
            {}
        },
        // KING: does not fork defended pieces
        ForkTestCase{
            "8/8/2rr4/8/3K4/8/8/8 w - - 0 1",
            {}
        },
        // KING: forks with capture
        ForkTestCase{
            "8/8/8/6k1/6P1/5NB1/8/8 b - - 0 1",
            {
                ForkFrame{"g3", "f3", "g4", "g5"}
            }
        },
        // KING: doesn't fork pieces already attacked
        ForkTestCase{
            "8/8/8/3nn3/3K4/8/8/8 w - - 0 1",
            {}
        }
    }
};

bool evaluate_pawn_fork_test_case(const ForkTestCase * tc) {
    return evaluate_fork_test_case(tc, PAWN);
}

bool evaluate_king_fork_test_case(const ForkTestCase * tc) {
    return evaluate_fork_test_case(tc, KING);
}

bool test_pawn_fork_hook() {
    return evaluate_test_set(&pawn_fork_hook_test_cases, &evaluate_pawn_fork_test_case);
}

bool test_king_fork_hook() {
    return evaluate_test_set(&king_fork_hook_test_cases, &evaluate_king_fork_test_case);
}
