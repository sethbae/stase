#include <string>
#include <vector>
#include <board.h>

#include "../../../game/cands/cands.h"
#include "../../test.h"

struct TestCaseWeak : TestCase {
    std::string fen;
    Square sq;
    int expected_balance;
    int expected_min_w;
    int expected_min_b;

    ~TestCaseWeak() override = default;
};

std::vector<TestCase> test_cases{
    {},
    {},
    {},
    {},
};

bool evaluate_test_case(const TestCase * tc) {

    const TestCaseWeak* tcw = dynamic_cast<const TestCaseWeak*>(tc);

    int balance = 0, min_w = 0, min_b = 0;
    Board b = fen_to_board(tcw->fen);

    balance = capture_walk(b, tcw->sq, &min_w, &min_b);

    return (balance == tcw->expected_balance
    && min_w == tcw->expected_min_w
    && min_b == tcw->expected_min_b);
}

void test_weak() {
    evaluate_test_set(test_cases, &evaluate_test_case);
}
