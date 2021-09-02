#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../test.h"
#include "../../../game/heur/heur.h"

const StringTestSet cap_test_set = {
        "game-cands-weak-cap",
        {
                // starting position has none
                StringTestCase{
                        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        {}
                },
                // doesn't capture #1
                StringTestCase{
                        "8/6b1/8/8/3N4/2P5/8/8 b - - 0 1",
                        {}
                },
                // doesn't capture #2
                StringTestCase{
                        "3r4/3q4/8/3P4/8/1B6/8/8 b - - 0 1",
                        {}
                },
                // doesn't capture #3
                StringTestCase{
                        "8/8/8/8/1k2K3/1rpR3R/8/8 w - - 0 1",
                        {}
                },
                // prefers weaker piece #1
                StringTestCase{
                        "8/8/8/3r4/2P1P3/8/8/3Q4 w - - 0 1",
                        {"c4d5", "e4d5"}
                },
                // prefers weaker piece #2
                StringTestCase{
                        "3r4/8/8/5b2/8/3R4/8/8 b - - 0 1",
                        {"f5d3"}
                },
                // knight takes
                StringTestCase{
                        "2q5/8/1N6/5b2/8/3r4/8/8 w - - 0 1",
                        {"b6c8"}
                },
                // bishop doesn't take
                StringTestCase{
                        "8/6p1/5p2/3N4/8/8/8/B7 w - - 0 1",
                        {}
                },
                // rook takes
                StringTestCase{
                        "8/1r3q2/8/8/8/1Q6/8/8 b - - 0 1",
                        {"b7b3"}
                },
                // queen takes
                StringTestCase{
                        "8/5q2/8/8/8/1Q6/8/8 w - - 0 1",
                        {"b3f7"}
                },
                // takes better but defended piece
                StringTestCase{
                        "8/8/8/8/1k2K3/1rqR3R/8/8 w - - 0 1",
                        {"d3c3"}
                },
                // has two options #1
                StringTestCase{
                        "8/8/8/8/1k2K3/3R1b1R/8/8 w - - 0 1",
                        {"d3f3", "h3f3"}
                },
                // has two options #2
                StringTestCase{
                        "8/8/5N2/8/4q3/2N2k2/8/8 w - - 0 1",
                        {"c3e4", "f6e4"}
                },
        }
};

bool evaluate_test_case_weak_cap(const StringTestCase * tc) {
    Gamestate gs(fen_to_board(tc->fen));

    std::vector<Move> moves = cands(gs);

    std::vector<std::string> strings;

    for (Move m : moves) {
        strings.push_back(sqtos(m.from) + sqtos(m.to));
    }

    return assert_string_lists_equal(strings, tc->expected_results);
}

void test_weak_cap() {
    evaluate_test_set(&cap_test_set, &evaluate_test_case_weak_cap);
}
