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
        },
        SearchLineTestCase{
            // hung queen against Timo
            "r1b1kb1r/pp1p1ppp/4pn2/2q5/1n2P3/3Q1N2/PPPB1PPP/RN2KB1R w KQkq - 2 8",
            {
                SearchLineRequirement{
                    MUST_APPEAR,
                    "Qe3 Nxc2 Kd1 Nxe3"
                },
                SearchLineRequirement{
                    MUST_APPEAR,
                    "Qe3 Nxc2 Ke2 Nxe3"
                }
            }
        },
        SearchLineTestCase{
            // bizarre underpromotion
            "1r4k1/3Pq1p1/4p2p/4Pp2/5P2/3QN1P1/5KB1/4N3 w - - 3 48",
            {
                SearchLineRequirement{
                    MUST_APPEAR,
                    "d8=Q Rxd8"
                },
                SearchLineRequirement{
                    MUST_APPEAR,
                    "d8=R Rxd8"
                },
                SearchLineRequirement{
                    MUST_APPEAR,
                    "d8=B Rxd8"
                },
                SearchLineRequirement{
                    MUST_APPEAR,
                    "d8=N Rxd8"
                }
            }
        },
    }
};

bool test_past_blunders() {
    return evaluate_test_set(&past_blunders_test_set, &evaluate_search_line_test_case);
}
