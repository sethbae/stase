#include "search_tools.h"
#include "thread.h"
#include "metrics.h"
#include "../game/gamestate.hpp"

namespace __constants {

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

/**
 * Checks whether the engine abort flag has been set and gracefully exits if so.
 */
void check_abort() {
    if (abort_flag() || (get_node_limit() != -1 && node_count() >= get_node_limit())) {
        interrupt_execution(0);
    }
}

/**
 * Updates the terminal node of a given node according to whether the subtree rooted at the
 * given node has been fully explored (up to the threshold specified in search_tools.h)
 */
void update_terminal(SearchNode * node) {

    // all candidates explored
    if (!node->cand_set->empty()) { return; }

    // legal list has been added (and expanded). In check gamestates do not have had to reach the legal threshold.
    if (node->visit_count < __constants::LEGAL_THRESHOLD && !node->gs->in_check) { return; }

    // check children
    for (int i = 0; i < node->children.size(); ++i) {
        if (millipawn_diff(node->children[i]->score, node->best_child->score) < __constants::EXPLORATION_THRESHOLD) {
            if (!node->children[i]->terminal) {
                return;
            }
        }
    }

    node->terminal = true;
}
