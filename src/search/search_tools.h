#ifndef STASE_SEARCH_TOOLS_H
#define STASE_SEARCH_TOOLS_H

#include "search.h"

namespace __engine_params {

    /**
     * These constants control certain behaviours of the search algorithm.
     * THRESHOLDs determine at what visit count a set of candidates is expanded.
     * DEPTHs determine how deep a tree will be extended at once.
     * SWING_THRESHOLD determines after what deterioration a swing will be triggered.
     * QUIESS_THRESHOLD determines the value at which positions become non-quiescent.
     * EXPLORATION_THRESHOLD determines the margin within which inferior alternatives will be
     * considered before a node is marked terminal.
     */

    const int CRITICAL_THRESHOLD = 0;
    const int MEDIAL_THRESHOLD = 2;
    const int FINAL_THRESHOLD = 8;
    const int LEGAL_THRESHOLD = 256;

    const int SWING_THRESHOLD = 2000;  // (millipawns)
    const float QUIESS_THRESHOLD = 2.0f;
    const int EXPLORATION_THRESHOLD = 1500;  // (millipawns)

    const int BURST_DEPTH = 5;
    const int CRITICAL_DEPTH = 2;
    const int MEDIAL_DEPTH = 1;
    const int FINAL_DEPTH = 1;
    const int LEGAL_DEPTH = 1;

}

SearchNode *new_node(const Gamestate & gs, Move m);
SearchNode *new_node(const std::string &);

void update_score(SearchNode *);
Eval trust_score(SearchNode *, bool is_white);
std::vector<SearchNode *> retrieve_best_line(SearchNode *);
std::vector<SearchNode *> retrieve_trust_line(SearchNode *);

inline bool is_swing(const Eval a, const Eval b) {
    return millipawn_diff(a, b) > __engine_params::SWING_THRESHOLD;
}

void update_terminal(SearchNode *);

void check_abort();

void print_line(std::vector<SearchNode *> & line);
void record_tree_in_file(const std::string & filename, SearchNode * root);

#endif //STASE_SEARCH_TOOLS_H
