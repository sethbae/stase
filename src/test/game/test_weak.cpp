#include "../test.h"
#include "game.h"

struct WeakTestCase {

    const std::string fen;
    std::vector<Square> squares;
    std::vector<Move> moves;
    std::vector<Ptype> colours;
    std::vector<bool> expected_results;

};

struct UnsafeTestCase {

    const std::string fen;
    std::vector<Square> squares;
    std::vector<Move> moves;
    std::vector<bool> expected_results;

};

const TestSet<WeakTestCase> weak_test_cases = {

};

const TestSet<UnsafeTestCase> unsafe_test_cases = {

};

bool evaluate_weak_test_case(const WeakTestCase * tc) {

    const Gamestate gs(fen_to_board(tc->fen));

    for (int i = 0; i < tc->squares.size(); ++i) {

        Square s = tc->squares[i];
        Move m = tc->moves[i];

        if (is_sentinel(m)) {
            if (is_weak_square(gs, s, tc->colours[i]) != tc->expected_results[i]) {
                return false;
            }
        } else {
            if (would_be_weak_after(gs, s, tc->colours[i], m) != tc->expected_results[i]) {
                return false;
            }
        }
    }

    return true;
}

bool evaluate_unsafe_test_case(const UnsafeTestCase * tc) {

    const Gamestate gs(fen_to_board(tc->fen));

    for (int i = 0; i < tc->squares.size(); ++i) {

        Square s = tc->squares[i];
        Move m = tc->moves[i];

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
