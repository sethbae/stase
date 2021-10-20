#include <string>
#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../test.h"
#include "../../../game/heur/heur.h"

TestSet<StringTestCase> hook_test_cases = {
        "game-cands-knight-fork-hook",
        {
            StringTestCase{
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                {}
            },
            // basic undefended pieces #1
            StringTestCase{
                "8/2p3p1/8/8/3N4/8/8/8 w - - 0 1",
                {"e6"}
            },
            // basic undefended pieces #1
            StringTestCase{
                "8/8/8/8/4R1n1/3R4/8/8 b - - 0 1",
                {"f2"}
            },
            // basic undefended pieces #3
            StringTestCase{
                "8/b1p5/8/8/8/2N5/8/8 w - - 0 1",
                {"b5"}
            },
        }
};

bool evaluate_test_case_knight_fork_hook(const StringTestCase *tc) {

    Gamestate gs(fen_to_board(tc->fen));

    find_knight_forks(gs, 0, nullptr);

    std::vector<std::string> strings;

    for (FeatureFrame* ff = gs.feature_frames[knight_fork_hook.id]; ff->centre != SQUARE_SENTINEL; ++ff) {
        strings.push_back(sqtos(ff->centre));
    }

    // print_feature_frames(gs.feature_frames[0]);

    return assert_string_lists_equal(strings, tc->expected_results);
}

void test_knight_fork_hook() {
    evaluate_test_set(&hook_test_cases, &evaluate_test_case_knight_fork_hook);
}
