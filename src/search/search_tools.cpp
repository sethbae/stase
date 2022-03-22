#include "search_tools.h"
#include "thread.h"
#include "metrics.h"
#include "../game/gamestate.hpp"

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
    if (node->visit_count < __engine_params::LEGAL_THRESHOLD && !node->gs->in_check) { return; }

    // check children
    for (int i = 0; i < node->children.size(); ++i) {
        if (millipawn_diff(node->children[i]->score, node->best_child->score) < __engine_params::EXPLORATION_THRESHOLD) {
            if (!node->children[i]->terminal) {
                return;
            }
        }
    }

    node->terminal = true;
}
