#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../test.h"
#include "../../../game/heur/heur.h"
#include "../../../game/helper.h"

struct TestCase {

    std::string name() const {
        return "game-cands-weak-cap";
    }

    const std::string fen;
    const std::vector<std::string> moves;
};

std::vector<TestCase> cap_test_cases = {
        // starting position has none
        TestCase{
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                {}
        },
        // doesn't capture #1
        TestCase{
                "8/6b1/8/8/3N4/2P5/8/8 b - - 0 1",
                {}
        },
        // doesn't capture #2
        TestCase{
                "3r4/3q4/8/3P4/8/1B6/8/8 b - - 0 1",
                {}
        },
        // doesn't capture #3
        TestCase{
                "8/8/8/8/1k2K3/1rpR3R/8/8 w - - 0 1",
                {}
        },
        // prefers weaker piece #1
        TestCase{
            "8/8/8/3r4/2P1P3/8/8/3Q4 w - - 0 1",
            {"c4d5", "e4d5"}
        },
        // prefers weaker piece #2
        TestCase{
                "3r4/8/8/5b2/8/3R4/8/8 b - - 0 1",
                {"d8d3"}
        },
        // knight takes
        TestCase{
                "2q5/8/1N6/5b2/8/3r4/8/8 w - - 0 1",
                {"b6c8"}
        },
        // bishop doesn't take
        TestCase{
                "8/6p1/5p2/3N4/8/8/8/B7 w - - 0 1",
                {}
        },
        // rook takes
        TestCase{
                "8/1r3q2/8/8/8/1Q6/8/8 b - - 0 1",
                {"b7c3"}
        },
        // queen takes
        TestCase{
                "8/5q2/8/8/8/1Q6/8/8 w - - 0 1",
                {"f7c3"}
        },
        // takes better but defended piece
        TestCase{
                "8/8/8/8/1k2K3/1rqR3R/8/8 w - - 0 1",
                {"d3c3"}
        },
        // has two options #1
        TestCase{
                "8/8/8/8/1k2K3/3R1b1R/8/8 w - - 0 1",
                {"d3f3", "h3f3"}
        },
        // has two options #2
        TestCase{
                "8/8/5N2/8/4q3/2N2k2/8/8 w - - 0 1",
                {"c3e4", "f5e4"}
        },
};

bool evaluate_test_case_weak_cap(const TestCase * tc) {
    Gamestate gs(fen_to_board(tc->fen));

    std::vector<Move> moves = cands(gs);

    std::vector<std::string> strings;

    for (Move m : moves) {
        strings.push_back(sqtos(m.from) + sqtos(m.to));
    }

    return assert_string_lists_equal(strings, tc->moves);
}

void test_weak_cap() {
    evaluate_test_set(cap_test_cases, &evaluate_test_case_weak_cap);
}
