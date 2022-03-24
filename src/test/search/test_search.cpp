#include "../test.h"

bool test_search() {

    bool passed = true;

    passed = test_cand_list_ordering() && passed;
    passed = test_cand_list_visit_counts() && passed;
    passed = test_legal_moves_present() && passed;
    passed = test_terminal_not_deepened() && passed;
    passed = test_terminal_updated() && passed;

    return passed;

}
