#include "test.h"
#include "../search/engine.h"
#include <vector>
#include <iomanip>

const static std::vector<std::string> fens = {
    "r6k/pp2r2p/4Rp1Q/3p4/8/1N1P2R1/PqP2bPP/7K b - - 0 24",
    "5rk1/1p3ppp/pq3b2/8/8/1P1Q1N2/P4PPP/3R2K1 w - - 2 27",
    "r2qr1k1/b1p2ppp/pp4n1/P1P1p3/4P1n1/B2P2Pb/3NBP1P/RN1QR1K1 b - - 1 16",
    "r4rk1/pp3ppp/2n1b3/q1pp2B1/8/P1Q2NP1/1PP1PP1P/2KR3R w - - 0 15",
    "3q1rk1/1pp3pp/5p1P/4pPP1/rb1pP3/3P1N2/b1P1B3/2QK2RR b - - 7 26",
    "4r3/5pk1/1p3np1/3p3p/2qQ4/P4N1P/1P3RP1/7K w - - 6 34",
    "r2q1rk1/5ppp/1np5/p1b5/2p1B3/P7/1P3PPP/R1BQ1RK1 b - - 1 17",
    "2kr3r/pp3p2/4p2p/1N1p2p1/3Q4/1P1P4/2q2PPP/5RK1 b - - 1 20",
    "4r1k1/5ppp/r1p5/p1n1RP2/8/2P2N1P/2P3P1/3R2K1 b - - 0 21",
    "5r1k/pp4pp/5p2/1BbQp1r1/6K1/7P/1PP3P1/3R3R w - - 2 26"
};

bool integration_test_main() {
    for (int i = 0; i < fens.size(); ++i) {
        Engine engine =
            EngineBuilder::for_position(fens[i])
                .with_timeout(10)
                .build();
        engine.blocking_run();
        std::cout << std::left << std::setw(75) << fens[i] << ": ok\n";
    }
    std::cout << "All tests passed.\n";
    return true;
}
