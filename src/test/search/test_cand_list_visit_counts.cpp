#include "../test.h"
#include "search.h"
#include "../../search/search_tools.h"
#include "../../game/gamestate.hpp"

class CandListVisitCountObserver : public Observer {

private:
    bool passed = true;
    int failures = 0;
public:
    std::vector<std::string> diagnostics;

    void open_event(const SearchNode * node, const SearchEvent ev, const CandList * cand_list) {
        if (ev == DEEPEN) {
            switch (*cand_list) {
                case CRITICAL:
                    if (node->visit_count != __constants::CRITICAL_THRESHOLD) {
                        fail_test(node, cand_list);
                    }
                    break;
                case MEDIAL:
                    if (node->visit_count != __constants::MEDIAL_THRESHOLD) {
                        fail_test(node, cand_list);
                    }
                    break;
                case FINAL:
                    if (node->visit_count != __constants::FINAL_THRESHOLD) {
                        fail_test(node, cand_list);
                    }
                    break;
                case LEGAL:
                    if (node->visit_count != __constants::LEGAL_THRESHOLD) {
                        fail_test(node, cand_list);
                    }
                    break;
            }
        }
    }

    void close_event(const SearchNode *, const SearchEvent, const CandList *) {}
    void register_event(const SearchNode *, const SearchEvent) {}

    bool passed_test() {
        diagnostics.push_back(
            "There were "
            + std::to_string(failures)
            + " failures encountered.\n"
        );
        return passed;
    }

private:

    void fail_test(const SearchNode * node, const CandList * c) {
        passed = false;
        ++failures;
        diagnostics.push_back(
            board_to_fen(node->gs->board)
            + ": Deepened " + name(*c) + " with a visit count of " + std::to_string(node->visit_count) + ".\n"
        );
    }
};

const TestSet<std::string> cand_list_visit_count_test_set{
    "search-cand-list-visit-counts",
    {
        "r4R2/2qbkpp1/4p1p1/2p1p3/2P1P3/pP2B1P1/P1Q5/2KR1B2 b - - 0 25",
        "1nb4r/1pk3p1/7q/p1P2pp1/8/2N2P2/PPPB2PP/R3R1K1 b - - 0 22",
        "4r1k1/pp4pp/3p4/2nP4/2Kp4/8/PP3PPP/8 b - - 3 27"
    }
};

bool evaluate_cand_list_vc_test_case(const std::string * fen) {

    CandListVisitCountObserver obs;
    run_with_node_limit(*fen, 25000, obs);

    if (!obs.passed_test()) {
        for (const std::string & diag : obs.diagnostics) {
            std::cout << diag;
        }
        return false;
    }
    return true;
}

bool test_cand_list_visit_counts() {
    return evaluate_test_set(&cand_list_visit_count_test_set, &evaluate_cand_list_vc_test_case);
}
