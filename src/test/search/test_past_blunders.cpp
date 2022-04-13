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
            // ends line too early (misses capture)
            "1k4r1/pppnR1bp/8/Q5p1/2br4/2N3B1/1P3PPP/6K1 b - - 2 25",
            {
                SearchLineRequirement{
                    MUST_APPEAR,
                    "g4 Qxc7 Ka8 Rxd7"
                }
            }
        },
    }
};

bool test_past_blunders() {
    return evaluate_test_set(&past_blunders_test_set, &evaluate_search_line_test_case);
}
