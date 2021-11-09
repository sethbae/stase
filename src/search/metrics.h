#ifndef STASE_METRICS_H
#define STASE_METRICS_H

int node_count();

inline Move current_best_move(const SearchNode * root) {
    return root->best_child->move;
}

#endif //STASE_METRICS_H
