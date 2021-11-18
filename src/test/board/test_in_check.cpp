#include "../test.h"
#include "board.h"

struct CheckTestCase {
    const std::string fen;
};

const TestSet<CheckTestCase> in_check_test_set{
    "board-in-check",
    {
            CheckTestCase{"3k4/8/8/8/8/8/4p3/3K4 w K - 0 1"},  // pawn checking
            CheckTestCase{"3k4/2P5/8/8/8/8/8/3K4 b K - 0 1"},
            CheckTestCase{"3k4/3P4/8/8/8/8/3p4/3K4 w - - 0 1"}, // pawn not checking
            CheckTestCase{"3k4/3P4/8/8/8/8/3p4/3K4 b - - 0 1"},
            CheckTestCase{"3k4/8/8/8/8/5b2/8/3K4 w K - 0 1"},  // bishop checking
            CheckTestCase{"7k/8/5B2/8/8/8/8/3K4 b K - 0 1"},
            CheckTestCase{"1k6/8/8/8/8/5b2/4b3/6K1 w - - 0 1"}, // bishop not checking
            CheckTestCase{"1k6/3B4/2B5/8/8/8/8/6K1 b - - 0 1"},
            CheckTestCase{"1k6/8/4n3/8/3K4/8/8/8 w K - 0 1"},  // knight checking
            CheckTestCase{"1k6/8/2N5/8/3K4/8/8/8 b K - 0 1"},
            CheckTestCase{"1k6/8/3n4/8/3Kn3/8/3n4/8 w - - 0 1"}, // knight not checking
            CheckTestCase{"1k6/2N5/1N6/3N4/3K4/8/8/8 b - - 0 1"},
            CheckTestCase{"1k1r4/8/8/8/3K4/8/8/8 w K - 0 1"},  // rook checking
            CheckTestCase{"1k1R4/8/8/8/3K4/8/8/8 b K - 0 1"},
            CheckTestCase{"1kr1r3/8/8/8/3K4/7r/8/8 w - - 0 1"}, // rook not checking
            CheckTestCase{"1k6/2RR4/8/8/3K4/8/8/7R b - - 0 1"},
            CheckTestCase{"1k6/8/5q2/8/3K4/8/8/8 w K - 0 1"},  // queen checking
            CheckTestCase{"1k6/8/3Q4/8/3K4/8/8/8 b K - 0 1"},
            CheckTestCase{"1k4q1/8/2q5/5q2/3K4/1q6/8/8 w - - 0 1"}, // queen not checking
            CheckTestCase{"1k6/3Q4/Q7/3Q4/3K4/8/2Q5/2Q5 b - - 0 1"},
            CheckTestCase{"1k6/8/8/8/2n5/q2r4/2B5/1KR5 w - - 0 1"}, // medley no check
            CheckTestCase{"1k6/2rQN3/B1QB4/1P6/2n5/q2r4/RP6/1K6 b - - 0 1"},
            CheckTestCase{"1k6/2rQN3/B1QB4/1P6/2n5/q2r4/RP6/1K5q w K - 0 1"}, //medley check
            CheckTestCase{"1k6/P1rQN3/B1QB4/1P6/2n5/q2r4/RP6/1K5q w K - 0 1"}
    }
};

bool evaluate_check_test_case(const CheckTestCase * tc) {
    Board b = fen_to_board(tc->fen);
    return in_check_hard(b) == b.get_cas_ws();
}

bool test_in_check() {
    return evaluate_test_set(&in_check_test_set, &evaluate_check_test_case);
}
