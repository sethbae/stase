#include "../test.h"

void test_game() {
    test_alpha_covers();
    test_beta_covers();
    test_gamma_covers();
    test_weak_hook();
    test_development_hook();
    test_capture_piece();
    test_defend_square();
    test_develop_piece();
    test_knight_fork_hook();
    test_sliding_fork_hook();
}