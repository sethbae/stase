#include "test_search_helpers.h"
#include "../test.h"

const TestSet<SearchLineTestCase> past_crashes_test_set{
    "search-past-crashes",
    {
        SearchLineTestCase{"2r5/p5Qp/3b4/2n4k/8/1P4P1/PB5P/2R2RK1 b - - 0 30", {}},
        SearchLineTestCase{"8/8/8/6p1/5k2/8/5K2/8 w - - 0 52", {}},
//        SearchLineTestCase{"8/6p1/8/K7/3k4/1r6/8/1b6 w - - 20 74", {}},
        SearchLineTestCase{"r4rk1/q4ppp/p3p3/1bbp4/p2Q4/P3PNP1/1PP2PBP/2KR3R w - - 3 19", {}},
    }
};

bool test_past_crashes() {
    return evaluate_test_set(&past_crashes_test_set, &evaluate_search_line_test_case);
}
