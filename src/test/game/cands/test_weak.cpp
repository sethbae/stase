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

const int DEFAULT = 10*piece_value(W_KING);

std::vector<TestCase> test_cases{
        TestCase{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" , stosq("e4"), 0, DEFAULT, DEFAULT },
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
