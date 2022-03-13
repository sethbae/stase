#include "../../test.h"

const TestSet<HookTestCase> discovered_hook_test_set{
    "game-cands-discovered-hook",
    {
        HookTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {}
        }
    }
};

bool evaluate_discovered_hook_test_case(const HookTestCase * tc) {
    return evaluate_hook_test_case(&discovered_hook, tc);
}

bool test_discovered_hook() {
    return evaluate_test_set(&discovered_hook_test_set, &evaluate_discovered_hook_test_case);
}
