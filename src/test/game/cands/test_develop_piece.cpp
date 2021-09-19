#include "board.h"
#include "game.h"
#include "../../test.h"

#include <vector>

const TestSet<ResponderTestCase> develop_test_cases = {
        "game-cands-develop-piece",
        {
            ResponderTestCase{
                "",
                {},
                {}
            }
        }
};

bool evaluate_develop_test_case(const ResponderTestCase * tc) {
    return true;
}

void test_develop_piece() {
    evaluate_test_set(&develop_test_cases, &evaluate_develop_test_case);
}
