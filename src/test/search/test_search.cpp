#include "../test.h"

bool test_search() {

    bool passed = true;

    passed = test_cand_list_ordering() && passed;
    passed = test_cand_list_visit_counts() && passed;

    return passed;

}
