#include "../test.h"

bool test_game() {

    bool passed = true;

    // top level
    passed = test_pin_cache() && passed;
    passed = test_cands_sorting() && passed;

    // cands
    passed = test_unsafe_piece_hook() && passed;
    passed = test_development_hook() && passed;
    passed = test_capture_piece() && passed;
    passed = test_defend_square() && passed;
    passed = test_develop_piece() && passed;
    passed = test_knight_fork_hook() && passed;
    passed = test_sliding_fork_hook() && passed;
    passed = test_queen_fork_hook() && passed;
    passed = test_fork_responder() && passed;
    passed = test_pawn_fork_hook() && passed;
    passed = test_king_fork_hook() && passed;
    passed = test_trade_piece() && passed;
    passed = test_retreat_piece() && passed;
    passed = test_desperado_piece() && passed;
    passed = test_check_hook() && passed;
    passed = test_pin_skewer_hook() && passed;
    passed = test_king_pinned_pieces_hook() && passed;
    passed = test_promotion_hook() && passed;
    passed = test_discovered_hook() && passed;
    passed = test_approach_kings() && passed;

    // glogic
    passed = test_alpha_covers() && passed;
    passed = test_beta_covers() && passed;
    passed = test_gamma_covers() && passed;
    passed = test_weak_square() && passed;
    passed = test_unsafe_piece() && passed;
    passed = test_luft() && passed;

    return passed;
}
