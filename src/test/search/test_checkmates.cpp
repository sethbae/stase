#include "../test.h"
#include "test_search_helpers.h"

const TestSet<SearchLineTestCase> checkmate_test_set{
    "search-checkmate",
    {
        SearchLineTestCase{
            // Queen + Rook mate
            "8/8/4k3/8/3Q1R2/8/8/7K w - - 0 1",
            {
                SearchLineRequirement{
                    MUST_APPEAR,
                    "Rf6 Ke7 Qd6 Ke8 Rf8"
                }
            },
            30000  // nodes allowed
        },
        SearchLineTestCase{
            // Queen mate
            "8/6k1/8/5QK1/8/8/8/8 w - - 0 1",
            {
                SearchLineRequirement{
                    MUST_APPEAR,
                    ""
                }
            },
            500  // nodes allowed
        }
    }
};

bool test_checkmate() {
    return evaluate_test_set(&checkmate_test_set, &evaluate_search_line_test_case);
}


