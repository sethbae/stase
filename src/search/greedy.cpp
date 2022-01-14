#include <iostream>
#include "search.h"
#include "game.h"
#include "search_tools.h"
#include "metrics.h"

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
 */
void deepen(SearchNode * node, CandList cand_list, int depth) {

    // exit conditions
    check_abort();
    if (depth == 0) { return; }
    if (node->gs->has_been_mated) {
        node->score =
            node->gs->board.get_white()
              ? white_has_been_mated()
              : black_has_been_mated();
        node->best_child = nullptr;
        return;
    }

    // fetch list to extend
    const std::vector<Move> & list = node->cand_set->get_list(cand_list);
    if (list.empty()) {
        // we still need to recurse up to the given depth
        for (int i = 0; i < node->num_children; ++i) {
            deepen(node->children[i], cand_list, depth - 1);
        }
        update_score(node);
        return;
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

        delete node->children;
        node->children = new_ptr;
    }

    // create child for each move in the list (appending)
    int c = node->num_children;
    for (int i = 0; i < list.size(); ++i) {
        node->children[c + i] = new_node(*node->gs, list[i]);
        node->children[c + i]->score = heur(*node->children[c + i]->gs);
        node->children[c + i]->cand_set = cands(*node->children[c + i]->gs);
        // recurse as appropriate
        deepen(node->children[c + i], cand_list, depth - 1);
    }
    node->num_children += list.size();
    node->cand_set->clear_list(cand_list);
    update_score(node);
}

/**
 * Visits a node. Depending on how many times the node in question has been visited,
 * a different candidate list will be expanded. In many cases, nothing will change
 * about the node except to update its score.
 */
void visit_node(SearchNode * node) {

    if (node->gs->has_been_mated) {
        return;
    }

    switch (node->visit_count++) {
        case 0:
            // recursively extend critical until depth reached
            deepen(node, CRITICAL, 5);
            break;
        case 4:
            // extend medial just once
            deepen(node, MEDIAL, 1);
            break;
        case 8:
            // extend final just once
            deepen(node, FINAL, 1);
            break;
        case 32:
            // extend legal just once
            if (node->cand_set->legal.empty()) {
                add_legal_moves(node);
            }
            deepen(node, LEGAL, 1);
            update_score(node);
            break;
        default:
            update_score(node);
            break;
    }

}

/**
 * Follows the best line from root to leaf, visiting each node in reverse (first
 * the leaf node, then back up to the root. If a swing is detected at any point,
 * then the path from the leaf back up to the current node is visited again, at
 * most once.
 * The in_swing flag should be set to false.
 */
void visit_best_line(SearchNode * node, bool in_swing) {

    if (node == nullptr) { return; }

    Eval prior = node->score;

    // recurse first so that the line is visited bottom (deepest) first
    visit_best_line(node->best_child, in_swing);
    visit_node(node);

    if (!in_swing && is_swing(prior, node->score)) {
        visit_best_line(node, true);
    }

}

std::vector<Move> greedy_search(const std::string & fen, int cycles) {

    // set up the root node
    Gamestate root_gs(fen);
    SearchNode root{
            &root_gs,
            cands(root_gs),
            heur(root_gs),
            MOVE_SENTINEL,
            0,
            nullptr,
            nullptr
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

    std::vector<SearchNode *> best_line = retrieve_best_line(root);
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
