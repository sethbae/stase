#include <string>
#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../test.h"
#include "../../../game/heur/heur.h"
#include "../../../game/gamestate.hpp"

const TestSet<StringTestCase> hook_test_cases = {
    "game-cands-unsafe-piece-hook",
    {
            // starting pos
            StringTestCase{
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                    {},
            },
            // hanging piece pairs
            StringTestCase{  // bishop
                    "B7/8/8/8/8/8/8/7b w - - 0 1",
                    {"a8", "h1"}
            },
            StringTestCase{  // knight
                    "8/8/5N2/3n4/8/8/8/8 w - - 0 1",
                    {"d5", "f6"}
            },
            StringTestCase{  // queen
                    "q7/8/8/8/8/8/8/7Q w - - 0 1",
                    {"a8", "h1"}
            },
            StringTestCase{  // rook
                    "2R5/8/8/8/8/8/8/2r5 w - - 0 1",
                    {"c8", "c1"}
            },
            StringTestCase{  // pawn
                    "8/8/8/8/8/p7/1P6/8 w - - 0 1",
                    {"a3", "b2"}
            },
            // king takes piece
            StringTestCase{  // white king
                    "8/8/8/8/8/1r6/K7/8 w - - 0 1",
                    {"b3"}
            },
            StringTestCase{  // black king
                    "8/1k6/1Q6/8/8/8/8/8 w - - 0 1",
                    {"b6", "b7"}
            },
            // doubled rooks facing each other
            StringTestCase{
                    "4r2b/4r3/8/3p4/5P2/8/4R3/B3R3 w - - 0 1",
                    {"e2", "e7"},
            },
            // doubled rooks plus bishop in corner
            StringTestCase{
                    "7b/4r3/8/3p4/5P2/8/4R3/B3R3 w - - 0 1",
                    {"e7", "h8"},
            },
            // queen knight medley
            StringTestCase{
                    "1q6/k1N5/8/2KQ4/8/8/8/8 w - - 0 1",
                    {"c7"},
            },
            // queen knight medley #2
            StringTestCase{
                    "8/k1N5/q7/2KQ4/8/8/8/8 w - - 0 1",
                    {"a6"},
            },
            // hanging knight
            StringTestCase{
                    "8/k1N2n2/8/2KQ4/4B3/5P2/6P1/8 w - - 0 1",
                    {"f7"},
            },
            // pawn threatens knight
            StringTestCase{
                    "8/8/8/3p4/4N3/8/8/8 w - - 0 1",
                    {"e4"},
            },
            // does not take defended piece of same value
            StringTestCase{
                    "6b1/8/8/3N4/2P5/8/8/8 w - - 0 1",
                    {}
            },
            // does not take defended piece
            StringTestCase{
                    "8/8/8/8/1k2K3/1rpR3R/8/8 w - - 0 1",
                    {}
            },
            // does take defended piece of higher value
            StringTestCase{
                    "6b1/8/8/3Q4/2P5/8/8/8 w - - 0 1",
                    {"d5", "g8"}
            }
    }
};

bool evaluate_test_case_unsafe_piece_hook(const StringTestCase *tc) {

    Gamestate gs(tc->fen);
    create_piece_encountered_caches(gs.board);

    discover_feature_frames(gs, &unsafe_piece_hook);

    std::vector<std::string> strings;

    for (FeatureFrame* ff = gs.frames[unsafe_piece_hook.id]; !is_sentinel(ff->centre); ++ff) {
        strings.push_back(sqtos(ff->centre));
    }

    return assert_string_lists_equal(strings, tc->expected_results);
}

bool test_unsafe_piece_hook() {
    return evaluate_test_set(&hook_test_cases, &evaluate_test_case_unsafe_piece_hook);
}
