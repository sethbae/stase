#include "board.h"
#include "../test.h"
#include "../../game/glogic/glogic.h"
#include "../../board/board.hpp"

#include <vector>
#include <iostream>
using std::cout;

struct CoversTestCase {
    const std::string fen;
    const Square piece_sq;
    const Square to_sq;
    const bool expected;
};

const TestSet<CoversTestCase> alpha_tests = {
        "game-piece-alpha-covers",
        std::vector<CoversTestCase>{
                // queen correct
                CoversTestCase{
                        "1k6/8/6R1/8/4Q3/2B5/8/1K6 w - - 0 1",
                        stosq("e4"),
                        stosq("e8"),
                        true
                },
                // rook correct
                CoversTestCase{
                        "1k6/8/6R1/8/4Q3/2B5/8/1K6 w - - 0 1",
                        stosq("g6"),
                        stosq("c6"),
                        true
                },
                // bishop correct
                CoversTestCase{
                        "1k6/8/6R1/8/4Q3/2B5/8/1K6 w - - 0 1",
                        stosq("c3"),
                        stosq("e1"),
                        true
                },
                // queen wrong
                CoversTestCase{
                        "1k6/8/6R1/8/4Q3/2B5/8/1K6 w - - 0 1",
                        stosq("e4"),
                        stosq("f6"),
                        false
                },
                // rook wrong
                CoversTestCase{
                        "1k6/8/6R1/8/4Q3/2B5/8/1K6 w - - 0 1",
                        stosq("g6"),
                        stosq("h7"),
                        false
                },
                // bishop wrong
                CoversTestCase{
                        "1k6/8/6R1/8/4Q3/2B5/8/1K6 w - - 0 1",
                        stosq("c3"),
                        stosq("c4"),
                        false
                },
                // doesn't beta
                CoversTestCase{
                        "1k6/8/6R1/8/4Q3/2B5/8/1K6 w - - 0 1",
                        stosq("e4"),
                        stosq("g6"),
                        false
                },
                // doesn't gamma
                CoversTestCase{
                        "1k6/8/6R1/4Q3/8/2B5/8/1K6 w - - 0 1",
                        stosq("c3"),
                        stosq("h8"),
                        false
                }
        }
};

const TestSet<CoversTestCase> beta_tests = {
        "game-piece-beta-covers",
        std::vector<CoversTestCase>{
                // queen correct
                CoversTestCase{
                        "1k6/3q2r1/5b2/8/8/8/8/1K6 b - - 0 1",
                        stosq("d7"),
                        stosq("h3"),
                        true
                },
                // rook correct
                CoversTestCase{
                        "1k6/3q2r1/5b2/8/8/8/8/1K6 b - - 0 1",
                        stosq("g7"),
                        stosq("e7"),
                        true
                },
                // bishop correct
                CoversTestCase{
                        "1k6/3q2r1/5b2/8/8/8/8/1K6 b - - 0 1",
                        stosq("f6"),
                        stosq("c3"),
                        true
                },
                // queen wrong
                CoversTestCase{
                        "1k6/3q2r1/5b2/8/8/8/8/1K6 b - - 0 1",
                        stosq("d7"),
                        stosq("f4"),
                        false
                },
                // rook wrong
                CoversTestCase{
                        "1k6/3q2r1/5b2/8/8/8/8/1K6 b - - 0 1",
                        stosq("g7"),
                        stosq("a4"),
                        false
                },
                // bishop wrong
                CoversTestCase{
                        "1k6/3q2r1/5b2/8/8/8/8/1K6 b - - 0 1",
                        stosq("f6"),
                        stosq("e4"),
                        false
                },
                // does beta #1
                CoversTestCase{
                        "1k6/3q2r1/5b2/8/8/8/8/1K6 b - - 0 1",
                        stosq("d7"),
                        stosq("g7"),
                        true
                },
                // does beta #2
                CoversTestCase{
                        "1k6/3q2r1/5b2/8/8/8/8/1K6 b - - 0 1",
                        stosq("f6"),
                        stosq("g7"),
                        true
                },
                // doesn't gamma
                CoversTestCase{
                        "1k6/3q2r1/5b2/8/8/8/8/1K6 b - - 0 1",
                        stosq("f6"),
                        stosq("h8"),
                        false
                }
        }
};

const TestSet<CoversTestCase> gamma_tests = {
        "game-piece-gamma-covers",
        std::vector<CoversTestCase>{
                // queen correct
                CoversTestCase{
                        "1k6/3b4/8/3r4/8/8/3q4/1K6 b - - 0 1",
                        stosq("d2"),
                        stosq("h6"),
                        true
                },
                // rook correct
                CoversTestCase{
                        "1k6/3b4/8/3r4/8/8/3q4/1K6 b - - 0 1",
                        stosq("d5"),
                        stosq("b5"),
                        true
                },
                // bishop correct
                CoversTestCase{
                        "1k6/3b4/8/3r4/8/8/3q4/1K6 b - - 0 1",
                        stosq("d7"),
                        stosq("h3"),
                        true
                },
                // queen wrong
                CoversTestCase{
                        "1k6/3b4/8/3r4/8/8/3q4/1K6 b - - 0 1",
                        stosq("d2"),
                        stosq("d8"),
                        false
                },
                // rook wrong
                CoversTestCase{
                        "1k6/3b4/8/3r4/8/8/3q4/1K6 b - - 0 1",
                        stosq("d5"),
                        stosq("h4"),
                        false
                },
                // bishop wrong
                CoversTestCase{
                        "1k6/3b4/8/3r4/8/8/3q4/1K6 b - - 0 1",
                        stosq("d7"),
                        stosq("a5"),
                        false
                },
                // does gamma #1
                CoversTestCase{
                        "1k6/3b4/8/3r4/8/8/3q4/1K6 b - - 0 1",
                        stosq("d2"),
                        stosq("d7"),
                        true
                },
                // does gamma #2
                CoversTestCase{
                        "1k6/3b4/8/3r4/8/8/3q4/1K6 b - - 0 1",
                        stosq("d5"),
                        stosq("d1"),
                        true
                },
                // does(n't) gamma #3
                CoversTestCase{
                        "1k6/3b4/8/3r4/8/8/3q4/1K6 b - - 0 1",
                        stosq("d2"),
                        stosq("d8"),
                        false
                }
        }
};

bool evaluate_alpha_covers_test_case(const CoversTestCase * tc) {
    const Board board = fen_to_board(tc->fen);
    return tc->expected == alpha_covers(board, tc->piece_sq, tc->to_sq);
}

bool evaluate_beta_covers_test_case(const CoversTestCase * tc) {
    const Board board = fen_to_board(tc->fen);
    return tc->expected == beta_covers(board, tc->piece_sq, tc->to_sq);
}

bool evaluate_gamma_covers_test_case(const CoversTestCase * tc) {
    const Board board = fen_to_board(tc->fen);
    return tc->expected == gamma_covers(board, tc->piece_sq, tc->to_sq);
}

bool test_alpha_covers() {
    return evaluate_test_set(&alpha_tests, &evaluate_alpha_covers_test_case);
}

bool test_beta_covers() {
    return evaluate_test_set(&beta_tests, &evaluate_beta_covers_test_case);
}

bool test_gamma_covers() {
    return evaluate_test_set(&gamma_tests, &evaluate_gamma_covers_test_case);
}