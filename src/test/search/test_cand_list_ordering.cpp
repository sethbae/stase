#include "search.h"
#include "../../search/observers/observers.hpp"
#include "../../game/gamestate.hpp"
#include "../test.h"
#include "test_observer.h"

class CandListOrderingObserver : public TestObserver {

private:
    unsigned null_count = 0;

public:
    inline void open_event(const SearchNode * node, const SearchEvent ev, const CandList * cand_list) {

        // update the counters
        TestObserver::register_applicable_event();
        if (!node || !node->cand_set || !cand_list) {
            ++null_count;
            return;
        }

        if (ev == DEEPEN || ev == BURST_DEEPEN) {
            switch (*cand_list) {
                case CRITICAL:
                    break;
                case MEDIAL:
                    if (node->cand_set->critical.size() > 0) {
                        fail_test(node, cand_list);
                    }
                    break;
                case FINAL:
                    if (node->cand_set->critical.size() > 0
                        || node->cand_set->medial.size() > 0) {
                        fail_test(node, cand_list);
                    }
                    break;
                case LEGAL:
                    if (node->cand_set->critical.size() > 0
                        || node->cand_set->medial.size() > 0
                        || node->cand_set->final.size() > 0) {
                        fail_test(node, cand_list);
                    }
                    break;
            }
        }
    }

    inline bool passed_test() {
        if (!TestObserver::passed_test()) {
            return false;
        }
        double null_rate = null_count / visit_count;
        if (null_rate > 0.25) {
            diagnostics.push_back("Test failed because the observer received more than 25% of visits with nullptr.\n");
            return false;
        }
        return true;
    }

private:

    inline void fail_test(const SearchNode * node, const CandList * cand_list) {
        TestObserver::fail_test(node,
            "Deepened " + name(*cand_list) + " with CandSet sizes ("
                + std::to_string(node->cand_set->critical.size()) + ","
                + std::to_string(node->cand_set->medial.size()) + ","
                + std::to_string(node->cand_set->final.size()) + ","
                + std::to_string(node->cand_set->legal.size()) + ")\n");
    }

};

const TestSet<std::string> search_cand_list_test_set{
    "search-cand-list-ordering",
    {
        "r2qkbnr/pp1bpppp/3p4/2p5/4P3/5N2/PPPP1PPP/RNBQ1RK1 b kq - 1 5",
        "3r1rk1/p1q2ppp/2bbpn2/1pp5/8/1P1P1NP1/PBPN1P1P/R2Q1RK1 w - - 0 14",
        "8/8/R3pkp1/7p/3P4/8/pr6/6K1 b - - 1 55"
    }
};

bool evaluate_cand_list_test_case(const std::string * fen) {

    CandListOrderingObserver obs;
    run_with_node_limit(*fen, 25000, obs);

    if (!obs.passed_test()) {
        for (const std::string & diag : obs.diagnostics) {
            std::cout << diag;
        }
        return false;
    }
    return true;
}

bool test_cand_list_ordering() {
    return evaluate_test_set(&search_cand_list_test_set, &evaluate_cand_list_test_case);
}
