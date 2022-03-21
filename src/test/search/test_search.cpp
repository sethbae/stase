#include "../test.h"

bool test_search() {

    bool passed = true;

    passed = test_cand_list_ordering() && passed;

    return passed;

}
