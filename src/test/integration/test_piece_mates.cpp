#include "integration_test.h"

const TestSet<IntegrationTest> piece_mates_test_set{
    "integration-piece-mates",
    {
        // KRQ vs K
        IntegrationTest{
            "8/8/8/3qr3/8/4k3/8/6K1 w - - 0 1",
            ENDGAME,
            0.1,
            0.1,
            5,
            BLACK_WON
        },
        // KQ vs K (near edge)
        IntegrationTest{
            "2k5/8/8/2K3Q1/8/8/8/8 b - - 0 1",
            ENDGAME,
            0.1,
            0.1,
            10,
            WHITE_WON
        },
        // KQ vs K (near centre)
        IntegrationTest{
            "8/8/3k4/6Q1/3K4/8/8/8 w - - 0 1",
            ENDGAME,
            0.2,
            0.1,
            15,
            WHITE_WON
        },
        // KR vs K (near edge)
        IntegrationTest{
            "8/8/8/8/8/4k3/3r4/4K3 w - - 0 1",
            ENDGAME,
            0.3,
            0.1,
            30,
            BLACK_WON
        },
    }
};

bool test_piece_mates() {
    return evaluate_test_set(&piece_mates_test_set, &evaluate_integration_test);
}
