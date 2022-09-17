#ifndef STASE_TEST_H
#define STASE_TEST_H

#include <vector>
#include <iostream>
#include <iomanip>

#include "../../include/stase/board.h"
#include "../../include/stase/game.h"
#include "../../src/game/cands/cands.h"
#include "../../src/game/cands/hook.hpp"
#include "../../src/game/cands/responder.hpp"
#include "../../src/game/heur/heur.h"

// save some typing! Used for responders.
const int p = piece_value(W_PAWN);
const int kn = piece_value(W_KNIGHT);
const int b = piece_value(W_BISHOP);
const int r = piece_value(W_ROOK);
const int q = piece_value(W_QUEEN);
const int k = piece_value(W_KING);
const int none = piece_value(W_KING) * 2;

/**
 * This holds strings instead of squares for ease of test-case entry.
 */
struct ExpectedFeatureFrame {
    const std::string centre;
    const std::string secondary;
    const int conf_1;
    const int conf_2;
};


struct StringTestCase {
    const std::string fen;
    const std::vector<std::string> expected_results;
};

struct ResponderTestCase {
    const std::string fen;
    const std::vector<FeatureFrame> feature_frames;
    const std::vector<std::string> expected_results;
};

struct HookTestCase {
    const std::string fen;
    std::vector<ExpectedFeatureFrame> expected_frames;
};

template <typename T> struct TestSet {
    const std::string name;
    const std::vector<T> cases;
};


bool assert_string_lists_equal(const std::vector<std::string> &, const std::vector<std::string> &);
bool expected_frame_matches_actual(const ExpectedFeatureFrame &, const FeatureFrame &);
bool evaluate_responder_test_case(const Responder *p, const ResponderTestCase *);
bool evaluate_hook_test_case(const Hook &, const HookTestCase *);

Piece random_piece();
Piece random_piece(Colour);
Square random_square();
Move random_move();

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

inline bool evaluate_test_function(const std::string name, bool (*func)(void)) {

    bool passed = false;
    if (func) {
        passed = (*func)();
    }

    std::cout << std::left << std::setw(30) << name << ":   ";
    std::cout << (passed ? "passed\n" : "failed\n");

    if (!passed) {
        std::cout << "*****SOME TESTS FAILED***** (" << name << ")\n";
        return false;
    }
    return true;
}

bool test_board();
bool test_game();
bool test_search();
bool integration_test_main();

// board tests
bool test_pieces();
bool test_set_get_square();
bool test_read_write_fens();
bool test_in_check();
bool test_castling();
bool test_mutate_hard();
bool test_mutate();
bool test_move_scores();

// top level game tests
bool test_alpha_covers();
bool test_beta_covers();
bool test_gamma_covers();
bool test_kpinned_piece_list();
bool test_cands_sorting();

// hooks and responders
bool test_unsafe_piece_hook();
bool test_development_hook();
bool test_capture_piece();
bool test_defend_square();
bool test_develop_piece();
bool test_knight_fork_hook();
bool test_sliding_fork_hook();
bool test_queen_fork_hook();
bool test_pawn_fork_hook();
bool test_king_fork_hook();
bool test_fork_responder();
bool test_weak_square();
bool test_unsafe_piece();
bool test_trade_piece();
bool test_retreat_piece();
bool test_desperado_piece();
bool test_check_hook();
bool test_pin_skewer_hook();
bool test_king_pinned_pieces_hook();
bool test_promotion_hook();
bool test_discovered_hook();

// search tests
bool test_cand_list_ordering();
bool test_cand_list_visit_counts();
bool test_legal_moves_present();
bool test_terminal_not_deepened();
bool test_terminal_updated();
bool test_best_child_updated();
bool test_past_blunders();
bool test_past_crashes();
bool test_parent_pointer();

bool stress_test_main();

bool stress_test_individual_hook(std::vector<Gamestate> &, const Hook &);
bool stress_test_cands(std::vector<Gamestate> &);

#endif //STASE_TEST_H
