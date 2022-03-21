#include "../test.h"

bool test_search() {

    bool passed = true;

    passed = test_cand_list_ordering() && passed;
    passed = test_cand_list_visit_counts() && passed;
    passed = test_legal_moves_present() && passed;

    return passed;

}
