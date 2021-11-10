#ifndef STASE_METRICS_H
#define STASE_METRICS_H

int node_count();
void reset_node_count();

inline Move current_best_move(const SearchNode * root) {

    if (!root) { return MOVE_SENTINEL; }
    if (root->num_children == 0) { return MOVE_SENTINEL; }

    Eval best_score = root->children[0]->score;
    Move best_move = root->children[0]->move;

    if (root->gs->board.get_white()) {
        for (int i = 1; i < root->num_children; ++i) {
            if (root->children[i]->score > best_score) {
                best_score = root->children[i]->score;
                best_move = root->children[i]->move;
            }
        }
    } else {
        for (int i = 1; i < root->num_children; ++i) {
            if (root->children[i]->score < best_score) {
                best_score = root->children[i]->score;
                best_move = root->children[i]->move;
            }
        }
    }

    return best_move;
}

#endif //STASE_METRICS_H
