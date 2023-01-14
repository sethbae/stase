#include "search_tools.h"
#include <pthread.h>
#include "metrics.h"
#include "../game/gamestate.hpp"

/**
 * Checks whether the engine abort flag has been set and gracefully exits if so.
 */
void check_abort() {
    if (get_abort_flag() || (get_node_limit() != -1 && node_count() >= get_node_limit())) {
        interrupt_execution(0);
    }
}

/**
 * Check whether the distribution of visit counts between the given node's children is very uneven. This is defined
 * as the most-visited node having been visited more than the rest combined, if there are more than two nodes, and
 * having been visited more
 */
bool uneven_visit_distribution(const SearchNode * node) {

    int sum = 0, max = 0;

    for (const SearchNode * child : node->children) {
        int visits = child->visit_count;
        if (visits > max) {
            max = visits;
        }
        sum += visits;
    }

    if (node->children.size() > 2) {
        // considered uneven if the max is greater than all the rest combined
        return max > (sum - max);
    } else if (node->children.size() == 2) {
        // or twice that, if there are two nodes
        return max > 2*(sum - max);
    } else {
        // and never if there's one
        return false;
    }
}

/**
 * Updates the terminal node of a given node according to whether the subtree rooted at the
 * given node has been fully explored (up to the threshold specified in search_tools.h)
 */
void update_terminal(SearchNode * node, Observer & obs) {

    if (!node) { return; }

    obs.register_event(node, UPDATE_TERMINAL);

    if (node->gs->game_over) {
        node->terminal = true;
        return;
    }

    // all candidates explored
    if (!node->cand_set->empty() || node->terminal) {
        return;
    }

    // legal list has been added (and expanded). In check gamestates do not have had to reach the legal threshold.
    if (node->visit_count <= __engine_params::LEGAL_THRESHOLD && !node->gs->in_check) {
        return;
    }

    // check children
    for (int i = 0; i < node->children.size(); ++i) {
        if (millipawn_diff(node->children[i]->score, node->best_child->score) < __engine_params::TERMINAL_MARGIN) {
            if (!node->children[i]->terminal) {
                return;
            }
        }
    }

    node->terminal = true;
}

/**
 * Checks that a subtree of the given depth rooted at the given root has visit counts
 * of at least the given threshold, ignoring terminal nodes and nodes with no candidates.
 */
bool check_vc_subtree(SearchNode * root, int threshold, int depth) {
    if (depth == 0 || root->terminal || root->cand_set->empty()) {
        return true;
    }
    if (root->visit_count < threshold) {
        return false;
    }
    for (int i = 0; i < root->children.size(); ++i) {
        if (!check_vc_subtree(root->children[i], threshold, depth - 1)) {
            return false;
        }
    }
    return true;
}

/**
 * Determines whether or not a soft exit is warranted. This takes into account the number of nodes searched,
 * the number of children the root has, and the gap between the best and second best option. See __engine_params
 * in search_tools.h for specifics.
 */
bool soft_exit_criteria(SearchNode * root) {

    if (node_count() < __engine_params::SOFT_EXIT_NODE_COUNT) { return false; }
    if (!root->best_child) { return false; }
    if (root->children.size() <= 1) { return true; }

    Eval second_best;
    if (root->best_child == root->children[0]) {
        second_best = root->children[1]->score;
    } else {
        second_best = root->children[0]->score;
    }

    for (int i = 0; i < root->children.size(); ++i) {
        if (root->children[i] != root->best_child) {
            Eval score = root->children[i]->score;
            if (root->gs->board.get_white() && score > root->score) {
                second_best = score;
            } else if (!root->gs->board.get_white() && score < root->score) {
                second_best = score;
            }
        }
    }

    if (millipawn_diff(root->best_child->score, second_best) < __engine_params::SOFT_EXIT_EVAL_MARGIN) {
        return false;
    }

    return check_vc_subtree(root, __engine_params::SOFT_EXIT_EXPLORED_VC, __engine_params::SOFT_EXIT_EXPLORED_DEPTH);
}

/**
 * Interrupts the calling thread. Immediately exits with no cleanup.
 */
void interrupt_execution(int) {
    pthread_exit(nullptr);
}

bool engine_abort = false;
bool get_abort_flag() { return engine_abort; }
void abort_analysis() { engine_abort = true; }
void reset_abort_flag() { engine_abort = false; }

int max_nodes = -1;
int get_node_limit() { return max_nodes; }
void set_node_limit(int limit) { max_nodes = limit; }
void clear_node_limit() { max_nodes = -1; }
