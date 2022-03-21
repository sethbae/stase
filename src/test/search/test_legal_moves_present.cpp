#include "../test.h"
#include "search.h"
#include "board.h"
#include "../../game/gamestate.hpp"

class LegalMovesObserver : public Observer {

public:
    std::vector<std::string> diagnostics;

    void open_event(const SearchNode *, const SearchEvent, const CandList * = nullptr) {}
    void close_event(const SearchNode *, const SearchEvent, const CandList * = nullptr) {}
    void register_event(const SearchNode *, const SearchEvent) {}

    bool passed_test() { return true; }

};

const TestSet<std::string> legal_moves_test_set{
    "search-legal-moves-present",
    {}
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
