#ifndef STASE_SEARCH_TOOLS_H
#define STASE_SEARCH_TOOLS_H

#include "../../include/stase/search.h"

namespace __engine_params {

    /**
     * These thresholds determine after how many visits a node expands the corresponding set of candidates.
     */
    const int CRITICAL_THRESHOLD = 0;
    const int MEDIAL_THRESHOLD = 2;
    const int FINAL_THRESHOLD = 8;
    const int LEGAL_THRESHOLD = 256;

    /**
     * SWING_THRESHOLD determines how big a swing is required to trigger a BURST_DEEPEN event.
     */
    const int SWING_THRESHOLD = 2000;  // (millipawns)

    /**
     * QUIESS_THRESHOLD determines when a position becomes viewed as non-quiescent.
     */
    const float QUIESS_THRESHOLD = 2.0f;

    /**
     * In order for a node to be marked terminal, all children within TERMINAL_MARGIN millipawns of the
     * best child must be already marked as terminal.
     */
    const int TERMINAL_MARGIN = 1500;  // (millipawns)

    /**
     * These depths determine how large a tree is extended from a node when the corresponding candidate
     * list is expanded.
     */
    const int BURST_DEPTH = 5;
    const int CRITICAL_DEPTH = 2;
    const int MEDIAL_DEPTH = 1;
    const int FINAL_DEPTH = 1;
    const int LEGAL_DEPTH = 1;

    /**
     * Soft exits are applicable when further exploration is unlikely to change the move selected.
     * The engine will not exit if fewer than SOFT_EXIT_NODE_COUNT nodes have been expanded, or if
     * there is a second-best move within SOFT_EXIT_EVAL_MARGIN millipawns of the best option.
     * It also checks that a subtree of depth SOFT_EXIT_EXPLORED_DEPTH has been explored with visit counts
     * of at least SOFT_EXIT_EXPLORED_VC.
     */
    const int SOFT_EXIT_NODE_COUNT = 25000;
    const int SOFT_EXIT_EVAL_MARGIN = 2000;  // (millipawns)
    const int SOFT_EXIT_EXPLORED_DEPTH = 4;
    const int SOFT_EXIT_EXPLORED_VC = MEDIAL_THRESHOLD;
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
bool soft_exit_criteria(SearchNode * root);

void check_abort();

void abort_analysis();
bool get_abort_flag();
void reset_abort_flag();

void set_node_limit(int nodes);
int get_node_limit();
void clear_node_limit();

void interrupt_execution(int);

void print_line(std::vector<SearchNode *> & line);
void record_tree_in_file(const std::string & filename, SearchNode * root);

#endif //STASE_SEARCH_TOOLS_H
