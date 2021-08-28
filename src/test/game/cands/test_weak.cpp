#include <string>
#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../test.h"
#include "../../../game/heur/heur.h"

struct TestCase {

    std::string name() {
        return "game-cands-weak";
    }

    std::string fen;
    Square sq;
    int expected_balance;
    int expected_min_w;
    int expected_min_b;

};

const int DEFAULT = 10 * piece_value(W_KING);

std::vector<TestCase> test_cases{
        TestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            stosq("e4"),
            0,
            DEFAULT,
            DEFAULT
        },
        TestCase{
            "4r2b/4r3/8/3p4/5P2/8/4R3/B3R3 w - - 0 1",
            stosq("e4"),
            -1,
            piece_value(W_ROOK),
            piece_value(B_PAWN),
        },
        TestCase{
            "4r2b/4r3/8/3p4/5P2/8/4R3/B3R3 w - - 0 1",
            stosq("e5"),
            1,
            piece_value(W_PAWN),
            piece_value(B_KNIGHT)
        },
        TestCase{
            "1q6/k1N5/8/2KQ4/8/8/8/8 w - - 0 1",
            stosq("a8"),
            0,
            piece_value(W_BISHOP),
            piece_value(B_QUEEN)
        },
        TestCase{
            "1q6/k1N5/8/2KQ4/8/8/8/8 w - - 0 1",
            stosq("b6"),
            -1,
            DEFAULT,
            piece_value(B_QUEEN),
        },
        TestCase{
            "8/k1N2n2/8/2KQ4/4B3/5P2/6P1/8 w - - 0 1",
            stosq("f3"),
            3,
            piece_value(W_PAWN),
            DEFAULT
        },
        TestCase{
            "1r3k2/2n2pp1/4pp2/3pp3/1ppp4/8/8/8 w - - 0 1",
            stosq("e8"),
            -3,
            DEFAULT,
            piece_value(B_KNIGHT)
        },
};

bool evaluate_test_case(const TestCase *tc) {

    int balance = 0, min_w = 0, min_b = 0;
    Board b = fen_to_board(tc->fen);

    balance = capture_walk(b, tc->sq, &min_w, &min_b);

    return (balance == tc->expected_balance
            && min_w == tc->expected_min_w
            && min_b == tc->expected_min_b);
}

void test_weak() {
    evaluate_test_set(test_cases, &evaluate_test_case);
}
