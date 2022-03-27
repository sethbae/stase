#include <cstring>
#include "test_search_helpers.h"

bool entire_line_present(SearchNode * root, std::vector<std::string> & move_strs) {

    SearchNode * current = root;

    for (const std::string move_str : move_strs) {

        bool present = false;
        SearchNode * next;

        for (SearchNode * child : current->children) {
            std::string child_move_str = mtos(current->gs->board, child->move);
            if (std::strcmp(move_str.c_str(), child_move_str.c_str()) == 0) {
                present = true;
                next = child;
                break;
            }
        }

        if (!present) {
            std::cout << "Required move was not present: " << move_str << "\n";
            return false;
        } else {
            current = next;
        }
    }
    return true;
}

bool evaluate_predicate(SearchLinePredicate pred, SearchNode * root, const std::string & move_str) {
    std::vector<std::string> moves;
    split(moves, move_str);
    switch (pred) {
        case MUST_APPEAR: return entire_line_present(root, moves);
        case MUST_NT_APPEAR: return !entire_line_present(root, moves);
    }
}

bool evaluate_search_line_test_case(const SearchLineTestCase * tc) {

    run_with_node_limit(tc->fen, 25000);
    SearchNode * root = fetch_root();

    for (const SearchLineRequirement req : tc->reqs) {
        if (!evaluate_predicate(req.pred, root, req.move_str)) {
            return false;
        }
    }
    return true;
}
