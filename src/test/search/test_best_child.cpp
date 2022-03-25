#include "../test.h"
#include "test_observer.h"
#include "search.h"

class BestChildUpdatedObserver : public TestObserver {

};

const TestSet<std::string> best_child_updated_test_set{
    "search-best-child-updated",
    {

    }
};

bool evaluate_best_child_updated_test_case(const std::string * fen) {
    BestChildUpdatedObserver obs;
    return evaluate_observer_test_case(fen, obs);
}

bool test_best_child_updated() {
    return evaluate_test_set(&best_child_updated_test_set, &evaluate_best_child_updated_test_case);
}
