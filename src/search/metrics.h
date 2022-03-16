#ifndef STASE_METRICS_H
#define STASE_METRICS_H

#include "search_tools.h"
#include "observer.hpp"

int node_count();
void register_new_node();
void reset_node_count();

inline Move current_best_move(SearchNode * root) {

    if (!root) { return MOVE_SENTINEL; }
    if (root->children.size() == 0) { return MOVE_SENTINEL; }

    update_score(root);

    return root->best_trust_child->move;
}

#endif //STASE_METRICS_H
