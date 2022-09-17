#include "../test.h"
#include "test_search_helpers.h"
#include "search.h"

class ParentPointerObserver : public TestObserver {

    void open_event(const SearchNode * node, const SearchEvent, const CandList * = nullptr, const int = 0) {
        if (node->parent) {
            register_applicable_event();
            for (const SearchNode * parents_child : node->parent->children) {
                if (parents_child == node) {
                    return;
                }
            }
            fail_test(node, "parent pointed to did not contain child.\n");
        }
    }

};

const TestSet<std::string> parent_pointer_test_set{
    "search-parent-pointer",
    {
        "r3k2r/1bpq1p1p/1pnp2pb/p3p1N1/2B1P3/2NP3P/PPP2PP1/R2Q1RK1 b kq - 1 11",
        "6r1/1bp2k2/1p1p1b2/p3p3/5P2/3P3P/PP4P1/R5K1 w - - 1 24",
        "8/1b3k2/1p1p1b2/p1p1pP2/8/3P3P/PP6/1R3K1r w - - 2 28",
    }
};

bool evaluate_parent_pointer_test_case(const std::string * tc) {
    ParentPointerObserver obs;
    return evaluate_observer_test_case(tc, obs);
}

bool test_parent_pointer() {
    return evaluate_test_set(&parent_pointer_test_set, &evaluate_parent_pointer_test_case);
}
