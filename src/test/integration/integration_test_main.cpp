#include "../test.h"
#include "integration_test.h"

bool integration_test_main() {
    bool passed = true;

    passed = test_piece_mates() && passed;

    return passed;
}
