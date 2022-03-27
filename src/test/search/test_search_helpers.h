#ifndef STASE_TEST_SEARCH_HELPERS_H
#define STASE_TEST_SEARCH_HELPERS_H

#include "search.h"
#include "../../game/gamestate.hpp"

enum SearchLinePredicate {
    MUST_APPEAR,
    MUST_NT_APPEAR
};

struct SearchLineRequirement{
    SearchLinePredicate pred;
    const std::string move_str;
};

struct SearchLineTestCase {
    const std::string fen;
    std::vector<SearchLineRequirement> reqs;
};

bool evaluate_search_line_test_case(const SearchLineTestCase *);

class TestObserver : public Observer {

protected:
    unsigned visit_count = 0;
    bool passed = true;

public:
    std::vector<std::string> diagnostics;

    inline virtual bool passed_test() {
        if (visit_count == 0) {
            diagnostics.push_back("Test failed because the observer was not visited.\n");
            return false;
        }
        diagnostics.push_back(
                "There were "
                + std::to_string(diagnostics.size())
                + " errors encountered.\n"
        );
        return passed;
    }

protected:

    inline void register_applicable_event() {
        ++visit_count;
    }

    inline void fail_test(const SearchNode * node, const std::string error) {
        diagnostics.push_back(
            board_to_fen(node->gs->board)
            + ": "
            + error
        );
        passed = false;
    }
};

template <typename T>
bool evaluate_observer_test_case(const std::string * fen, T & obs) {

    run_with_node_limit(*fen, 25000, obs);

    if (!obs.passed_test()) {
        for (const std::string & diag : obs.diagnostics) {
            std::cout << diag;
        }
        return false;
    }
    return true;
}

#endif //STASE_TEST_SEARCH_HELPERS_H
