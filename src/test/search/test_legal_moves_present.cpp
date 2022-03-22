#include "../test.h"
#include "search.h"
#include "board.h"
#include "../../game/gamestate.hpp"

class LegalMovesObserver : public Observer {

private:
    bool passed = true;
    int failures = 0;
    int total = 0;
public:
    std::vector<std::string> diagnostics;

    void close_event(const SearchNode * node, const SearchEvent ev, const CandList * cand_list) {

        if (!cand_list) { return; }
        if (ev == DEEPEN || ev == BURST_DEEPEN) {
            if (*cand_list == LEGAL) {

                ++total;
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

    bool passed_test() {
        if (!total) {
            diagnostics.push_back(
                "Test failed: no cases at all were encountered with legal moves.\n"
            );
            return false;
        }
        diagnostics.push_back(
                "There were " + std::to_string(failures) + " errors encountered.\n"
        );
        return passed;
    }

private:
    void fail_test(const SearchNode * node, const Move m) {
        passed = false;
        ++failures;
        if (!is_sentinel(m)) {
            diagnostics.push_back(
                board_to_fen(node->gs->board)
                + ": "
                + move2uci(m)
                + " was not found among children.\n"
            );
        } else {
            // if the move given was sentinel, it means too many children were found
            diagnostics.push_back(
                board_to_fen(node->gs->board)
                + ": more children were found than there exist legal moves.\n"
            );
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
    run_with_node_limit(*fen, 25000, obs);

    if (!obs.passed_test()) {
        for (const std::string & diag : obs.diagnostics) {
            std::cout << diag;
        }
        return false;
    }
    return true;
}

bool test_legal_moves_present() {
    return evaluate_test_set(&legal_moves_test_set, &evaluate_legal_moves_test_case);
}