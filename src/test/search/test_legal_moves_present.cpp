#include "test_search_helpers.h"
#include "../test.h"
#include "search.h"
#include "board.h"

class LegalMovesObserver : public TestObserver {

public:
    void close_event(const SearchNode * node, const SearchEvent ev, const CandList * cand_list) {

        if (!cand_list) { return; }
        if (ev == DEEPEN || ev == BURST_DEEPEN) {
            if (*cand_list == LEGAL) {

                TestObserver::register_applicable_event();
                std::vector<Move> expected_legals = legal_moves(node->gs->board);

                for (int i = 0; i < expected_legals.size(); ++i) {
                    bool present = false;
                    for (int j = 0; j < node->children.size(); ++j) {
                        if (equal_exactly(expected_legals[i], node->children[j]->move)) {
                            present = true;
                            break;
                        }
                    }
                    if (!present) {
                        fail_test(node, expected_legals[i]);
                    }
                }

                if (expected_legals.size() < node->children.size()) {
                    // MOVE_SENTINEL is used to signify that there are too many children
                    fail_test(node, MOVE_SENTINEL);
                }
            }
        }
    }

private:
    void fail_test(const SearchNode * node, const Move m) {
        if (!is_sentinel(m)) {
            TestObserver::fail_test(node, move2uci(m) + " was not found among children.\n");
        } else {
            // if the move given was sentinel, it means too many children were found
            TestObserver::fail_test(node, "more children were found than there exist legal moves.\n");
        }
    }
};

const TestSet<std::string> legal_moves_test_set{
    "search-legal-moves-present",
    {
        "8/8/R3pkp1/7p/3P4/8/pr6/6K1 b - - 1 55",
        "r2k3r/2pp1p1p/p1bb4/1p2p1P1/2B5/1PP1P2P/P2BK3/5R2 b - - 0 25",
    }
};

bool evaluate_legal_moves_test_case(const std::string * fen) {
    LegalMovesObserver obs;
    return evaluate_observer_test_case(fen, obs);
}

bool test_legal_moves_present() {
    return evaluate_test_set(&legal_moves_test_set, &evaluate_legal_moves_test_case);
}
