#include "../test.h"
#include "search.h"
#include "test_observer.h"

class TerminalObserver : public TestObserver {

public:
    void open_event(const SearchNode * node, const SearchEvent ev, const CandList * cand_list) {
        if (ev == DEEPEN) {
            TestObserver::register_applicable_event();
            if (node->terminal) {
                TestObserver::fail_test(node, "deepened a terminal node.\n");
            }
        }
    }

};

const TestSet<std::string> terminal_test_set{
    "search-terminal-not-deepened",
    {
        "8/3kp2p/3p2p1/3Pp3/1p1n2P1/7P/7r/4K3 b - - 0 45",
        "4r3/5kp1/3Q3p/4P3/4BP2/6P1/4NK2/8 w - - 1 62",
        "3r4/1R6/pp4k1/5p2/1P2pP1P/3p2P1/2b2K2/3q4 b - - 2 38"
    }
};

bool evaluate_terminal_test_case(const std::string * fen) {
    TerminalObserver obs;
    return evaluate_observer_test_case(fen, obs);
}

bool test_terminal_not_deepened() {
    return evaluate_test_set(&terminal_test_set, &evaluate_terminal_test_case);
}
