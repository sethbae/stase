#include "../test.h"

bool test_board(void) {

    bool passed = true;

//    passed = test_pieces() && passed;
//    passed = test_set_get_square() && passed;
    passed = test_read_write_fens() && passed;
//    passed = test_in_check() && passed;
//    passed = test_castling() && passed;
//    passed = test_mutate() && passed;
//    passed = test_mutate_hard() && passed;

    return passed;
}
