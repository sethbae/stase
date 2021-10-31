#ifndef STASE_TEST_H
#define STASE_TEST_H

#include <vector>
#include <iostream>
#include <iomanip>

#include "board.h"
#include "game.h"
#include "../../src/game/cands/cands.h"
#include "../../src/game/heur/heur.h"

// save some typing! Used for responders.
const int p = piece_value(W_PAWN);
const int kn = piece_value(W_KNIGHT);
const int b = piece_value(W_BISHOP);
const int r = piece_value(W_ROOK);
const int q = piece_value(W_QUEEN);
const int k = piece_value(W_KING);
const int none = piece_value(W_KING) * 2;


struct StringTestCase {
    const std::string fen;
    const std::vector<std::string> expected_results;
};

struct ResponderTestCase {
    const std::string fen;
    const std::vector<FeatureFrame> feature_frames;
    const std::vector<std::string> expected_results;
};

template <typename T> struct TestSet {
    const std::string name;
    const std::vector<T> cases;
};

bool assert_string_lists_equal(const std::vector<std::string> &, const std::vector<std::string> &);

template <typename T>
inline bool evaluate_test_set(const TestSet<T> * test_set, bool (*func)(const T*)) {

    if (test_set->cases.empty()) { return true; }

    int success = 0, failed = 0, first_failure = -1;

    for (T tc : test_set->cases) {
        if ((*func)(&tc)) {
            ++success;
        } else {
            ++failed;
            first_failure =
                    (first_failure < 0)
                    ? success
                    : first_failure;
        }
    }

    std::cout << std::left << std::setw(30) << test_set->name << ":   ";
    std::cout << success << "/" << success + failed << "\n";

    if (failed) {
        std::cout << "*****SOME TESTS FAILED***** ("
                    << test_set->name << "@" << first_failure << ")\n";
        return false;
    }

    return true;

}

inline bool evaluate_responder_test_case(const Responder * resp, const ResponderTestCase * tc) {

    Gamestate gs(fen_to_board(tc->fen));
    Move moves[MAX_MOVES_PER_HOOK];
    IndexCounter move_counter(MAX_MOVES_PER_HOOK);

    // run the responder on the feature frames
    for (FeatureFrame ff : tc->feature_frames) {
        resp->resp(gs.board, &ff, &moves[0], move_counter);
    }

    // convert the output to a vector of strings
    std::vector<std::string> strings;
    for (int i = 0; i < move_counter.idx(); ++i) {
        strings.push_back(sqtos(moves[i].from) + sqtos(moves[i].to));
    }

    // check that they match
    return assert_string_lists_equal(strings, tc->expected_results);
}

void test_board();
void test_game();

void test_unsafe_piece_hook();
void test_development_hook();
void test_capture_piece();
void test_defend_square();
void test_alpha_covers();
void test_beta_covers();
void test_gamma_covers();
void test_develop_piece();
void test_knight_fork_hook();
void test_sliding_fork_hook();
void test_queen_fork_hook();
void test_kp_fork_hook();
void test_weak_square();
void test_unsafe_piece();
void test_trade_piece();

void stress_test_main();

bool stress_test_individual_hook(std::vector<Gamestate> &, const Hook *);
bool stress_test_cands(std::vector<Gamestate> &);

#endif //STASE_TEST_H
