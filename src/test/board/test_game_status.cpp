#include "board.h"
#include "../test.h"

struct GameStatusTestCase {
    const std::vector<std::string> fen;
    const GameStatus expected_status;
};

const TestSet<GameStatusTestCase> game_status_test_set{
    "board-game-status",
    {
        GameStatusTestCase{
            { starting_fen() },
            ONGOING
        },
        GameStatusTestCase{
            { "rnbqkbnr/pppppQpp/8/8/2B5/8/PPPPPPPP/RNB1K1NR b KQkq - 0 1" },
            WHITE_WON
        },
        GameStatusTestCase{
            { "rn1qkbnr/pppppppp/8/8/2b5/4PnPN/PPPP1P1P/RNBRKB1R w KQkq - 0 1" },
            BLACK_WON
        },
        GameStatusTestCase{
                { "1Q6/8/k1p5/2p2N2/2P5/1PB2K2/8/8 b - - 4 64" },
                DRAW_BY_STALEMATE
        },
        GameStatusTestCase{
            { "8/8/3b4/8/3k4/6r1/3KR3/8 w - - 50 109" },
            DRAW_BY_FIFTY_MOVES
        },
        GameStatusTestCase{
            {
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PNnP1/q6P/1N1R1K2 b - - 11 36",
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PN1P1/q6n/1N1R1K2 w - - 0 37",
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PN1P1/q6n/1N1RK3 b - - 1 37",
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PNnP1/q7/1N1RK3 w - - 2 38",
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PNnP1/q7/1N1R1K2 b - - 3 38",
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PN1P1/q6n/1N1R1K2 w - - 4 39",
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PN1P1/q6n/1N1RK3 b - - 5 39",
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PNnP1/q7/1N1RK3 w - - 6 40",
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PNnP1/q7/1N1R1K2 b - - 7 40",
                "1Q2nk2/5pp1/4p2p/2p5/1pP2P2/3PN1P1/q6n/1N1R1K2 w - - 8 41",
            },
            DRAW_BY_THREEFOLD
        }
    }
};

bool evaluate_game_status_test_case(const GameStatusTestCase * tc) {
    return game_rules::check_status(tc->fen) == tc->expected_status;
}

bool test_game_status() {
    return evaluate_test_set(&game_status_test_set, &evaluate_game_status_test_case);
}
