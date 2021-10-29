#include "../test.h"
#include "game.h"
#include <string.h>

struct WeakTestCase {

    const std::string fen;
    std::vector<std::string> squares;
    std::vector<std::string> moves;
    std::vector<std::string> colours;
    std::vector<bool> expected_results;

};

struct UnsafeTestCase {

    const std::string fen;
    std::vector<std::string> squares;
    std::vector<std::string> moves;
    std::vector<bool> expected_results;

};

const TestSet<WeakTestCase> weak_test_cases = {
    "game-piece-weak-square",
    {
        WeakTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {"e1", "e8", "a1", "h8", "e4", "e4"},
            {"none", "none", "none", "none", "none", "none"},
            {"white", "black", "white", "black", "white", "black"},
            {false, false, false, false, false, false}
        }
    }
};

const TestSet<UnsafeTestCase> unsafe_test_cases = {

};

bool evaluate_weak_test_case(const WeakTestCase * tc) {

    const Gamestate gs(fen_to_board(tc->fen));

    for (int i = 0; i < tc->squares.size(); ++i) {

        Square s = stosq(tc->squares[i]);

        Move m;
        if (strcmp(tc->moves[i].c_str(), "none") == 0) {
            m = MOVE_SENTINEL;
        } else {
            m = Move{
                    stosq(tc->moves[i].substr(0,2)),
                    stosq(tc->moves[i].substr(2,4))
            };
        }

        Ptype colour =
            strcmp(tc->colours[i].c_str(), "white") == 0
                ? WHITE
                : BLACK;

        if (is_sentinel(m)) {
            if (is_weak_square(gs, s, colour) != tc->expected_results[i]) {
                return false;
            }
        } else {
            if (would_be_weak_after(gs, s, colour, m) != tc->expected_results[i]) {
                return false;
            }
        }
    }

    return true;
}

bool evaluate_unsafe_test_case(const UnsafeTestCase * tc) {

    const Gamestate gs(fen_to_board(tc->fen));

    for (int i = 0; i < tc->squares.size(); ++i) {

        Square s = stosq(tc->squares[i]);

        Move m;
        if (strcmp(tc->moves[i].c_str(), "none") == 0) {
            m = MOVE_SENTINEL;
        } else {
            m = Move{
                stosq(tc->moves[i].substr(0,2)),
                stosq(tc->moves[i].substr(2,4))
            };
        }

        if (is_sentinel(m)) {
            if (is_unsafe_piece(gs, s) != tc->expected_results[i]) {
                return false;
            }
        } else {
            if (would_be_unsafe_after(gs, s, m) != tc->expected_results[i]) {
                return false;
            }
        }
    }

    return true;
}

void test_weak_square() {
    evaluate_test_set(&weak_test_cases, &evaluate_weak_test_case);
}

void test_unsafe_piece() {
    evaluate_test_set(&unsafe_test_cases, &evaluate_unsafe_test_case);
}
