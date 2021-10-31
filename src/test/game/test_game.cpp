#include "../test.h"

void test_game() {

    // piece
    test_alpha_covers();
    test_beta_covers();
    test_gamma_covers();
    test_weak_square();
    test_unsafe_piece();

    // cands
    test_unsafe_piece_hook();
    test_development_hook();
    test_capture_piece();
    test_defend_square();
    test_develop_piece();
    test_knight_fork_hook();
    test_sliding_fork_hook();
    test_queen_fork_hook();
    test_kp_fork_hook();
    test_trade_piece();
}