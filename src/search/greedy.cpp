#include <iostream>
#include "search.h"
#include "game.h"
#include "search_tools.h"
#include "../game/gamestate.hpp"
#include "metrics.h"

const int CRITICAL_THRESHOLD = 0;
const int MEDIAL_THRESHOLD = 2;
const int FINAL_THRESHOLD = 8;
const int LEGAL_THRESHOLD = 256;

// the point at which a position will be considered not quiescent
const float QUIESS_THRESHOLD = 2.0f;

const int BURST_DEPTH = 5;
const int CRITICAL_DEPTH = 2;
const int MEDIAL_DEPTH = 1;
const int FINAL_DEPTH = 1;
const int LEGAL_DEPTH = 1;

inline bool is_present_in_list(const std::vector<Move> & list, const Move m) {
    for (int i = 0; i < list.size(); ++i) {
        if (equal(list[i], m)) {
            return true;
        }
    }
    return false;
}

/**
 * Extends the CandSet of the given node to include legal moves. It removes from this list
 * all legal moves which (i) are already present in another list (ii) belong to an existing
 * child of this node.
 */
void add_legal_moves(SearchNode * node) {

    std::vector<Move> legals = legal_moves(node->gs->board);
    std::vector<Move> approved;
    approved.reserve(legals.size());

    for (int i = 0; i < legals.size(); ++i) {

        // check in existing lists
        if (is_present_in_list(node->cand_set->critical, legals[i])
            || is_present_in_list(node->cand_set->medial, legals[i])
            || is_present_in_list(node->cand_set->final, legals[i])) {
            continue;
        }

        // check in children
        bool already_created = false;
        for (int j = 0; j < node->num_children; ++j) {
            if (equal(node->children[j]->move, legals[i])) {
                already_created = true;
                break;
            }
        }

        if (!already_created) {
            approved.push_back(legals[i]);
        }
    }

    node->cand_set->legal = approved;
}

/**
 * For each node in the given candidate list of the given node, this creates a new node
 * in the tree. If the depth given is greater than 1, then this will be done recursively
 * to create a new subtree of size equal to the depth.
 * If given a depth of 0, nothing will happen.
 * Returns true if new nodes are created, false otherwise.
 */
bool deepen(SearchNode * node, CandList cand_list, int depth, bool burst=false) {

    // exit conditions
    check_abort();
    if (node->gs->has_been_mated) { return false; }

    if (depth == 0) {

        if (burst) { return false; }

        if (quiess(*node->gs) >= QUIESS_THRESHOLD) {
            return deepen(node, CRITICAL, BURST_DEPTH, true);
        } else {
            return false;
        }
    }

    if (burst && quiess(*node->gs) < QUIESS_THRESHOLD) {
        return false;
    }

    // if no early exit, this counts as a visit
    ++node->visit_count;

    // fetch list to extend
    const std::vector<Move> & list = node->cand_set->get_list(cand_list);
    if (list.empty()) {
        // we still need to recurse up to the given depth
        bool changes = false;
        for (int i = 0; i < node->num_children; ++i) {
            changes = deepen(node->children[i], cand_list, depth - 1, burst)
                        || changes;
        }
        update_score(node);
        return changes;
    }

    // if the node hasn't yet been extended at all, allocate *all* its children pointers
    if (node->num_children == 0) {
        node->children = new SearchNode *[node->cand_set->size()];
    }

    // if we're extending legal moves, we need to re-allocate and copy the child pointers
    if (cand_list == LEGAL) {
        int all_kids = node->num_children + list.size();
        SearchNode ** new_ptr = new SearchNode *[all_kids];

        for (int i = 0; i < node->num_children; ++i) {
            new_ptr[i] = node->children[i];
        }

        delete[] node->children;
        node->children = new_ptr;
    }

    // create child for each move in the list (appending)
    int c = node->num_children;
    for (int i = 0; i < list.size(); ++i) {
        node->children[c + i] = new_node(*node->gs, list[i]);
        node->children[c + i]->cand_set = cands(*node->children[c + i]->gs, new CandSet);
        if (node->children[c + i]->gs->has_been_mated) {
            node->children[c + i]->score =
                node->children[c + i]->gs->board.get_white()
                    ? white_has_been_mated()
                    : black_has_been_mated();
        } else {
            node->children[c + i]->score = heur(*node->children[c + i]->gs);
        }
    }
    node->num_children += list.size();
    node->cand_set->clear_list(cand_list);

    // recurse as appropriate
    bool changes = (node->num_children != c);
    for (int i = 0; i < node->num_children; ++i) {
        changes = deepen(node->children[i], cand_list, depth - 1, burst)
                    || changes;
    }

    update_score(node);
    return changes;
}

/**
 * Visits a node. Depending on how many times the node in question has been visited,
 * a different candidate list will be expanded. In many cases, nothing will change
 * about the node except to update its score.
 * Returns true if new nodes were deepened and false otherwise.
 */
bool visit_node(SearchNode * node) {

    if (node->gs->has_been_mated) {
        return false;
    }

    switch (node->visit_count) {
        case CRITICAL_THRESHOLD:
            return deepen(node, CRITICAL, CRITICAL_DEPTH);
        case MEDIAL_THRESHOLD:
            return deepen(node, MEDIAL, MEDIAL_DEPTH);
        case FINAL_THRESHOLD:
            return deepen(node, FINAL, FINAL_DEPTH);
        case LEGAL_THRESHOLD:
            if (node->cand_set->legal.empty()) {
                add_legal_moves(node);
            }
            return deepen(node, LEGAL, LEGAL_DEPTH);
        default:
            ++node->visit_count;
            update_score(node);
            return false;
    }

}

/**
 * Visits a node and forces the next unexplored list to be extended, excluding
 * legal moves. If that list is empty, it moves on to the next one, still
 * excluding legal moves.
 * Returns true if any new nodes were in fact extended.
 */
bool force_visit(SearchNode * node) {

    bool changes = false;

    if (node->visit_count <= CRITICAL_THRESHOLD) {
        changes = deepen(node, CRITICAL, CRITICAL_DEPTH);
        node->visit_count = CRITICAL_THRESHOLD + 1;
    }

    if (!changes && node->visit_count < MEDIAL_THRESHOLD) {
        changes = deepen(node, MEDIAL, MEDIAL_DEPTH);
        node->visit_count = MEDIAL_THRESHOLD + 1;
    }

    if (!changes && node->visit_count < FINAL_THRESHOLD) {
        changes = deepen(node, FINAL, FINAL_DEPTH);
        node->visit_count = FINAL_THRESHOLD + 1;
    }

    return changes;
}

/**
 * Follows the best line from root to leaf, force-visiting each node in order
 * from the leaf back up to the root. The scores are updated after the extension,
 * so the line visited is that which is the best line in the initial position.
 * Returns true if any changes were made, false otherwise.
 */
bool force_visit_best_line(SearchNode * node) {

    if (node == nullptr) { return false; }

    bool changes = force_visit(node->best_child);
    changes = force_visit(node) || changes;

    return changes;
}

/**
 * Follows the best line from root to leaf, visiting each node in reverse (first
 * the leaf node, then back up to the root. If a swing is detected at any point,
 * then the path from the leaf back up to the current node is visited again, at
 * most once.
 * The in_swing flag should be set to false.
 * Returns true if any descendant of the given node was materially updated (ie new
 * moves + nodes), and false otherwise.
 */
bool visit_best_line(SearchNode * node, bool in_swing) {

    if (node == nullptr) { return false; }

    Eval prior = node->score;

    // recurse first so that the line is visited bottom (deepest) first
    bool changes = visit_best_line(node->best_child, in_swing);
    changes = visit_node(node) || changes;

    if (!in_swing && is_swing(prior, node->score)) {
        changes = force_visit_best_line(node) || changes;
    }

    return changes;

}

std::vector<Move> greedy_search(const std::string & fen, int cycles) {

    // set up the root node
    Gamestate root_gs(fen);
    SearchNode root{
            &root_gs,
            cands(root_gs, new CandSet),
            heur(root_gs),
            MOVE_SENTINEL,
            0,
            nullptr,
            nullptr,
            nullptr,
            0
    };

    return greedy_search(&root, cycles);

}

std::vector<Move> greedy_search(SearchNode * root, int cycles) {

    if (root->score == zero()) {
        root->score = heur(*root->gs);
    }
    if (root->cand_set->empty()) {
        delete root->cand_set;
        root->cand_set = cands(*root->gs, new CandSet);
    }

    int i = 0;

    auto start = std::chrono::high_resolution_clock::now();

    while (i++ < cycles || cycles < 0) {
        visit_best_line(root, false);

//        auto stop = std::chrono::high_resolution_clock::now();
//        long duration = duration_cast<std::chrono::microseconds>(stop - start).count();
//        double seconds = ((double)duration) / 1000000.0;
//
//        std::cout << i << ": (" << ((double) node_count()) / seconds << ") ";
//        std::vector<SearchNode *> best_line = retrieve_best_line(root);
//        print_line(best_line);
    }

    std::vector<SearchNode *> best_line = retrieve_trust_line(root);
    print_line(best_line);

    std::vector<Move> moves;
    for (SearchNode * s : best_line) {
        if (s != root) {
            moves.push_back(s->move);
        }
    }

    std::string name = "stase_tree";
    //record_tree_in_file(name, root);

    return moves;
}

Eval trust_score(SearchNode * node, bool is_white) {

    if (is_mate(node->score)) {
        return node->score;
    }

    int penalty = 0;
    if (node->visit_count <= 1) { penalty = 5000; }
    else if (node->visit_count == 2) { penalty = 1500; }
    else if (node->visit_count < 4) { penalty = 400; }
    else if (node->visit_count < 8) { penalty = 100; }

    if (is_white) { return node->score - penalty; }
    else { return node->score + penalty; }
}
