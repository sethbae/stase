#include <string>
#include <vector>
#include <board.h>

struct TestCase {
    std::string fen;
    Square sq;
    int expected_balance;
    int expected_min_w;
    int expected_min_b;
};

std::vector<TestCase> test_cases{
    {},
    {},
    {},
    {},
};

void test_weak() {

    int success = 0, failed = 0;

    for (const TestCase & tc : test_cases) {
        int balance = 0, min_w = 0, min_b = 0;

        Board b = fen_to_board(tc.fen);

        balance = control_walk(b, tc.sq, &min_w, &min_b);

    }
}
