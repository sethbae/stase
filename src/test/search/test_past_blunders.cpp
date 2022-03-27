#include "../test.h"
#include "test_search_helpers.h"

const TestSet<SearchLineTestCase> past_blunders_test_set{
    "search-past-blunders",
    {
        SearchLineTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {
                SearchLineRequirement{
                    MUST_APPEAR,
                    "e4 c5"
                },
                SearchLineRequirement{
                    MUST_NT_APPEAR,
                    "e4 c5 Ke2"
                }
            }
        }
    }
};

bool test_past_blunders() {
    return evaluate_test_set(&past_blunders_test_set, &evaluate_search_line_test_case);
}
