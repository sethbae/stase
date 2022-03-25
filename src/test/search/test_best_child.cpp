#include "../test.h"
#include "test_observer.h"
#include "search.h"

class BestChildUpdatedObserver : public TestObserver {

public:
    void close_event(const SearchNode * node, const SearchEvent ev, const CandList * cand_list) {

        if (!node || node->children.empty()) { return; }

        TestObserver::register_applicable_event();

        // check that the best child has the score the node says it does
        if (node->score != node->best_child->score) {
            TestObserver::fail_test(node, " node's score differs from the score of the best child.\n");
        }

        // check that there is no child with a better score than the node's score
        for (int i = 0; i < node->children.size(); ++i) {
            Eval score = node->children[i]->score;
            if (node->gs->board.get_white() && score > node->score) {
                fail_test(node, node->best_child, node->children[i]);
            } else if (!node->gs->board.get_white() && score < node->score) {
                fail_test(node, node->best_child, node->children[i]);
            }
        }
    }

private:
    void fail_test(const SearchNode * node, const SearchNode * was, const SearchNode * should_ve_been) {
        TestObserver::fail_test(node,
            " best_child was "
            + to_string(was)
            + " but should've been "
            + to_string(should_ve_been)
            + "\n"
        );
    }
};

const TestSet<std::string> best_child_updated_test_set{
    "search-best-child-updated",
    {
        "rnbqkb1r/pp4pp/2p2n2/3p1p2/8/3P1N2/PPP1BPPP/RNBQ1RK1 w kq - 0 7",
        "rnbqk2r/3nbppp/p2pp3/2p3P1/4P3/3PBP2/PPPQ3P/2KR1BNR b kq - 2 11",
        "3r1rk1/5q1p/p5p1/8/8/PPQ4P/6B1/4R1RK w - - 1 36"
    }
};

bool evaluate_best_child_updated_test_case(const std::string * fen) {
    BestChildUpdatedObserver obs;
    return evaluate_observer_test_case(fen, obs);
}

bool test_best_child_updated() {
    return evaluate_test_set(&best_child_updated_test_set, &evaluate_best_child_updated_test_case);
}
