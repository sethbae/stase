#include "../test.h"
#include "search.h"
#include "test_observer.h"
#include "../../search/search_tools.h"

class TerminalNotDeepenedObserver : public TestObserver {

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

class TerminalUpdatedCorrectlyObserver : public TestObserver {

public:
    void close_event(const SearchNode * node, const SearchEvent ev, const CandList * cand_list) {
        TestObserver::register_applicable_event();

        // create a copy of the node, but with terminal false. Then update terminal,
        // and see if the expected value matches the actual.
        SearchNode copy{
            node->gs,
            node->cand_set,
            node->score,
            node->move,
            false,
            node->children,
            node->best_child,
            node->best_trust_child,
            node->visit_count
        };
        update_terminal(&copy);

        if (node->terminal != copy.terminal) {
            if (!node->terminal) {
                fail_test(node, "should be terminal but is not.\n");
            }
        }
    }

};

std::vector<std::string> terminal_test_fens{
    "8/3kp2p/3p2p1/3Pp3/1p1n2P1/7P/7r/4K3 b - - 0 45",
    "4r3/5kp1/3Q3p/4P3/4BP2/6P1/4NK2/8 w - - 1 62",
    "3r4/1R6/pp4k1/5p2/1P2pP1P/3p2P1/2b2K2/3q4 b - - 2 38"
};

const TestSet<std::string> terminal_not_deepened_test_set{
    "search-terminal-not-deepened",
    terminal_test_fens
};

const TestSet<std::string> terminal_updated_test_set{
    "search-terminal-updated",
    terminal_test_fens
};

bool evaluate_terminal_not_deepened_test_case(const std::string * fen) {
    TerminalNotDeepenedObserver obs;
    return evaluate_observer_test_case(fen, obs);
}

bool test_terminal_not_deepened() {
    return evaluate_test_set(&terminal_not_deepened_test_set, &evaluate_terminal_not_deepened_test_case);
}

bool evaluate_terminal_updated_test_case(const std::string * fen) {
    TerminalUpdatedCorrectlyObserver obs;
    return evaluate_observer_test_case(fen, obs);
}

bool test_terminal_updated() {
    return evaluate_test_set(&terminal_updated_test_set, &evaluate_terminal_updated_test_case);
}
