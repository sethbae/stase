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


Move unpack_four_char_san(const std::string &);
bool assert_string_lists_equal(const std::vector<std::string> &, const std::vector<std::string> &);
bool expected_frame_matches_actual(const ExpectedFeatureFrame &, const FeatureFrame &);
bool evaluate_responder_test_case(const Responder *p, const ResponderTestCase *);
bool evaluate_hook_test_case(const Hook *, const HookTestCase *);

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

bool test_board();
bool test_game();

bool test_pieces();
bool test_set_get_square();
bool test_read_write_fens();
bool test_in_check();
bool test_castling();
bool test_mutate_hard();
bool test_mutate();

bool test_unsafe_piece_hook();
bool test_development_hook();
bool test_capture_piece();
bool test_defend_square();
bool test_alpha_covers();
bool test_beta_covers();
bool test_gamma_covers();
bool test_kpinned_piece_list();

bool test_develop_piece();
bool test_knight_fork_hook();
bool test_sliding_fork_hook();
bool test_queen_fork_hook();
bool test_kp_fork_hook();
bool test_weak_square();
bool test_unsafe_piece();
bool test_trade_piece();
bool test_retreat_piece();
bool test_desperado_piece();
bool test_check_hook();
bool test_pin_skewer_hook();
bool test_pin_skewer_resp();
bool test_king_pinned_pieces_hook();

bool stress_test_main();

bool stress_test_individual_hook(std::vector<Gamestate> &, const Hook *);
bool stress_test_cands(std::vector<Gamestate> &);

#endif //STASE_TEST_H
