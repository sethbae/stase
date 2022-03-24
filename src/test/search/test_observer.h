#ifndef STASE_TEST_OBSERVER_H
#define STASE_TEST_OBSERVER_H

#include "search.h"

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

    inline void fail_test(const SearchNode * node, const std::string & error) {
        diagnostics.push_back(
            board_to_fen(node->gs->board)
            + ": "
            + error
        );
        passed = false;
    }
};


#endif //STASE_TEST_OBSERVER_H
