#include <iostream>
#include "search.h"
#include "game.h"
#include "search_tools.h"
#include "metrics.h"

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
        node->score = node->gs->board.get_white()
                      ? white_has_been_mated()
                      : black_has_been_mated();
        node->best_child = nullptr;
        return;
    }

    // fetch list to extend
    const std::vector<Move> & list = node->cand_set.get_list(cand_list);
    if (list.empty()) { return; }

    // if the node hasn't yet been extended at all, allocate *all* its children pointers
    if (node->num_children == 0) {
        node->children = new SearchNode *[node->cand_set.size()];
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
    node->cand_set.clear_list(cand_list);
    update_score(node);
}

void visit_node(SearchNode * node) {

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
            // extend legal just once (STUB)
            break;
        default:
            break;
    }

}

void visit_best_line(SearchNode * node) {

    if (node == nullptr) { return; }

    // recurse first so that the line is visited bottom (deepest) first
    visit_best_line(node->best_child);
    visit_node(node);

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
    if (root->cand_set.empty()) {
        root->cand_set = cands(*root->gs);
    }

    int i = 0;

    auto start = std::chrono::high_resolution_clock::now();

    while (cycles < 0 || i++ < cycles) {
        visit_best_line(root);

        auto stop = std::chrono::high_resolution_clock::now();
        long duration = duration_cast<std::chrono::microseconds>(stop - start).count();
        double seconds = ((double)duration) / 1000000.0;

        std::cout << i << ": (" << ((double) node_count()) / seconds << ") ";
        std::vector<SearchNode *> best_line = retrieve_best_line(root);
        print_line(best_line);
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
    // record_tree_in_file(name, &root);

    return moves;
}
