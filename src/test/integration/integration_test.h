#ifndef STASE_INTEGRATION_TEST_H
#define STASE_INTEGRATION_TEST_H

#include <string>
#include "../test.h"
#include "../../search/engine.h"
#include "../../search/engine_client.h"

class IntegrationTest {
public:
    const std::string fen;
    const GamePhase phase;
    const int primary_think_time;
    const int secondary_think_time;
    const int max_moves;
    const GameStatus expected_result;

    IntegrationTest(
            const std::string & fen,
            const GamePhase phase,
            const int primary_think_time,
            const int secondary_think_time,
            const int max_moves,
            const GameStatus expected_result)
        :  fen(fen),
           phase(phase),
           primary_think_time(primary_think_time),
           secondary_think_time(secondary_think_time),
           max_moves(max_moves),
           expected_result(expected_result) {}
};

inline bool evaluate_integration_test(const IntegrationTest & test) {

    bool is_primary = false;
    EngineClient engine(test.fen, test.phase);

    for (int unused = 0; unused < test.max_moves; ++unused) {
        const std::string move =
            engine.get_computer_move(
                (is_primary = !is_primary)
                    ? test.primary_think_time
                    : test.secondary_think_time);
        GameStatus status = value_of(move);
        if (status == test.expected_result) {
            return true;
        } else if (status != ONGOING) {
            std::cout << "unexpected termination: " << move << "\n";
            return false;
        }
    }

    std::cout << "game did not end\n";
    return false;
}

#endif //STASE_INTEGRATION_TEST_H
