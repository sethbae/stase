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
            500  // nodes allowed
        }
    }
};

bool test_checkmate() {
    return evaluate_test_set(&checkmate_test_set, &evaluate_search_line_test_case);
}


