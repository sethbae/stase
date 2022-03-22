#include <string>
#include <vector>
#include <board.h>

#include "../../../game/cands/cands.h"
#include "../../test.h"
#include "../../../game/gamestate.hpp"
#include "fork_helpers.h"

TestSet<SlidingForkTestCase> sliding_fork_hook_test_cases = {
    "game-cands-sliding-fork-hook",
    {
        SlidingForkTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {}
        },
        // BISHOP: basic undefended pieces #1
        SlidingForkTestCase{
            "6r1/8/8/5B2/2r5/8/8/8 w - - 0 1",
            {
                SlidingForkFrame{"c4", "g8", NOT_ATTACKED_AT_ALL}
            }
        },
        // BISHOP: basic undefended pieces #2
        SlidingForkTestCase{
            "8/8/3b3N/8/8/8/8/2K5 b - - 0 1",
            {
                SlidingForkFrame{"c1", "h6", NOT_ATTACKED_AT_ALL}
            }
        },
        // BISHOP: finds forks despite unsafe square
        SlidingForkTestCase{
            "8/8/3b3N/8/8/6P1/7P/2K5 b - - 0 1",
            {
                SlidingForkFrame{"c1", "h6", NOT_ATTACKED_AT_ALL},
                SlidingForkFrame{"h2", "h6", NOT_ATTACKED_AT_ALL}
            }
        },
        // BISHOP: does not fork minimum value, defended sliding pieces
        // ie, the knight here, because the constraint can never be met
        SlidingForkTestCase{
            "5r2/8/8/p7/1n6/4B3/8/8 w - - 0 1",
            {}
        },
        // BISHOP: does not fork pieces which can move in that direction
        SlidingForkTestCase{
            "8/8/4Q3/8/8/1K3b2/8/8 b - - 0 1",
            {
                SlidingForkFrame{"b3", "e6", k}
            }
        },
        // BISHOP: does not fork pieces which can just immediately trade
        SlidingForkTestCase{
            "5r2/8/8/p7/1n6/4B3/8/8 w - - 0 1",
            {}
        },
        // BISHOP: doesn't fork pieces already attacked
        SlidingForkTestCase{
            "8/8/1K6/8/8/2b5/8/R7 b - - 0 1",
            {}
        },
        // BISHOP: fork is blocked by own piece
        SlidingForkTestCase{
            "n3n3/8/2P5/3B4/8/8/8/8 w - - 0 1",
            {
                // (the two knights in the background)
                SlidingForkFrame{"a8", "e8", NOT_ATTACKED_AT_ALL}
            }
        },
        // BISHOP: puzzle #2
        SlidingForkTestCase{
            "r1b2rk1/pp3ppp/2p3n1/8/2P5/q3B3/4BQPP/1R3RK1 w - - 0 21",
            {
                // the actual puzzle
                SlidingForkFrame{"a3", "f8", r},
                // incidental other forks
                SlidingForkFrame{"b7", "f7", NOT_ATTACKED_AT_ALL},
                SlidingForkFrame{"a3", "a7", NOT_ATTACKED_AT_ALL}
            }
        },
        // ROOK: basic undefended pieces #1
        SlidingForkTestCase{
            "4r3/8/8/1P4P1/8/8/8/8 w - - 0 1",
            {
                SlidingForkFrame{"b5", "g5", NOT_ATTACKED_AT_ALL}
            }
        },
        // ROOK: basic undefended pieces #2
        SlidingForkTestCase{
            "8/8/8/1b4n1/8/8/2R5/8 w - - 0 1",
            {
                SlidingForkFrame{"b5", "g5", NOT_ATTACKED_AT_ALL}
            }
        },
        // ROOK: potential fork found even from unsafe square
        SlidingForkTestCase{
            "8/8/1b6/1b4n1/8/8/2R5/8 w - - 0 1",
            {
                SlidingForkFrame{"b5", "g5", NOT_ATTACKED_AT_ALL}
            }
        },
        // ROOK: does not fork defended piece of lower/equal value
        SlidingForkTestCase{
            "8/8/3r4/8/8/2B3K1/1P6/8 b - - 0 1",
            {}
        },
        // ROOK: does not fork pieces which can move in that direction
        SlidingForkTestCase{
            "8/8/3r4/8/8/2R3K1/8/8 b - - 0 1",
            {}
        },
        // ROOK: doesn't fork pieces already attacked
        SlidingForkTestCase{
            "8/8/3r4/N7/8/3B4/8/8 w - - 0 1",
            {}
        },
        // ROOK: doesn't fork pieces already attacked
        SlidingForkTestCase{
            "8/8/8/8/8/2pr4/R4K2/8 w - - 0 1",
            {}
        },
//        // ROOK: puzzle #1
//        // TODO (ST-91): this fork involves interrupting a line of defence and isn't found
//        SlidingForkTestCase{
//            "4r2k/3q2p1/6Qp/p3p3/8/2P2R1P/6PK/8 w - - 2 37",
//            {
//                // the actual puzzle: not working
//                SlidingForkFrame{"d7", "g7", p},
//                // incidental other forks
//                // SlidingForkFrame{"h6", "h8", k}
//            }
//        },
        // ROOK: puzzle #2
        SlidingForkTestCase{
            "8/8/3p1k2/3R4/5qP1/5B2/5PK1/8 w - - 3 54",
            {
                // the actual puzzle
                SlidingForkFrame{"f4", "f6", k},
                // incidental other forks
                SlidingForkFrame{"d6", "f6", k},
                SlidingForkFrame{"d6", "f4", NOT_ATTACKED_AT_ALL},
                SlidingForkFrame{"g2", "g4", k},
            }
        },
        // ROOK: puzzle #3
        SlidingForkTestCase{
            "r4r1k/1pq1n1pp/2p1Q3/4p2P/4P3/1PP3P1/5P2/3R1RK1 w - - 1 26",
            {
                // the actual puzzle
                SlidingForkFrame{"c7", "e7", NOT_ATTACKED_AT_ALL},
                // incidental other forks
                SlidingForkFrame{"c7", "e5", NOT_ATTACKED_AT_ALL},
                SlidingForkFrame{"d1", "h5", r},
            }
        },
        // can fork a pinned piece along a direction it could normally move
        SlidingForkTestCase{
            "8/4k3/4b3/8/8/8/r7/4R2K w - - 0 1",
            {
                SlidingForkFrame{"a2", "e6", NOT_ATTACKED_AT_ALL}
            }
        }
    }
};

/*
 * Currently, forks which are also captures are not detected by the fork hook.
 * They should always be detected by the unsafe_piece hook, so it is an issue
 * of scoring/prioritising that candidate differently, not of finding/not finding
 * the move. Below are some test cases which can be used if the functionality ever
 * changes.
 *
 *  "8/8/8/4b3/8/2P5/8/R3K3 w - - 0 1"
 *  "rnbqkbnr/pp2pppp/2pp4/P5B1/8/N2P4/1PP1PPPP/R2QKBNR b KQkq - 0 1"
 *  "r3kbnr/p3pppp/n1p5/8/2P5/1P2PQ2/PB3PPP/RN3RK1 w - - 0 1"
 *  "7r/8/8/2Q1b3/8/6p1/8/8 w - - 0 1"
 *
 *        // ROOK: forks with capture
 *        SlidingForkTestCase{
 *            "7k/8/8/6Rp/6P1/8/8/7q w - - 0 1",
 *            {
 *                SlidingForkFrame{"h1", "h8", NOT_ATTACKED_AT_ALL}
 *            }
 *        },
 *
 *        // BISHOP: forks with capture
 *        SlidingForkTestCase{
 *            "8/8/8/4b3/8/2P5/8/R3K3 w - - 0 1",
 *            {
 *                SlidingForkFrame{"a1", "e1", k},
 *                SlidingForkFrame{"c3", "e1", k},
 *                SlidingForkFrame{"a1", "c3", NOT_ATTACKED_AT_ALL},
 *            }
 *        },
 *
 *        // BISHOP: puzzle #1
 *        SlidingForkTestCase{
 *            "rbr2k2/ppn2ppp/8/2p5/2P2P2/P5P1/R3N1BP/2K4R w - - 0 22",
 *            {
 *                // the actual puzzle: not working
 *                SlidingForkFrame{"a8", "c8", NOT_ATTACKED_AT_ALL},
 *                // incidental other forks threatened
 *                SlidingForkFrame{"a2", "c4", NOT_ATTACKED_AT_ALL},
 *                SlidingForkFrame{"c1", "c4", k},
 *                SlidingForkFrame{"c5", "f8", k},
 *            }
 *        },
 */

bool test_sliding_fork_hook() {
    return evaluate_test_set(&sliding_fork_hook_test_cases, &evaluate_sliding_fork_test_case);
}
