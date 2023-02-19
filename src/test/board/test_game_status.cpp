#include "board.h"
#include "../test.h"

struct GameStatusTestCase {
    const std::string fen;
    const GameStatus expected_status;
};

const TestSet<GameStatusTestCase> game_status_test_set{
    "board-game-status",
    {
        GameStatusTestCase{
            "",
            ONGOING
        }
    }
};

bool evaluate_game_status_test_case(const GameStatusTestCase * tc) {
    return true;
}

bool test_game_status() {
    return evaluate_test_set(&game_status_test_set, &evaluate_game_status_test_case);
}
