#include <iostream>
#include "search.h"
#include "game.h"
#include "search_tools.h"

bool expand_list(SearchNode *, CandList, int);
bool expand_node(SearchNode *, CandList, int);

std::vector<Move> greedy_search(const std::string & fen, int cycles) {

    // set up the root node
    Gamestate root_gs(fen_to_board(fen));
    SearchNode root{
            &root_gs,
            cands(root_gs),
            heur(root_gs),
            MOVE_SENTINEL,
            0,
            nullptr,
            nullptr
    };

}

std::vector<Move> greedy_search(SearchNode * root, int cycles) {

    if (root->score == zero()) {
        root->score = heur(*root->gs);
    }
    if (root->cand_set.empty()) {
        root->cand_set = cands(*root->gs);
    }

    int i = 0;

    while (cycles < 0 || i++ < cycles) {

//        std::vector<SearchNode *> best_line = retrieve_best_line(&root);
//        print_line(best_line);

//        std::cout << "critical!\n";

        expand_list(root, CRITICAL, 5);

//        std::cout << "medial!\n";

        if (expand_list(root, MEDIAL, 1)) { continue; }

//        std::cout << "final!\n";

        if (expand_list(root, FINAL, 1)) { continue; }

//        std::cout << "legal!\n";

        if (expand_list(root, LEGAL, 1)) { continue; }

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

/**
 * Follows the best child pointers from the given node until a leaf is reached. For each
 * node, it expands it according to the list and recursive flag given.
 */
//bool expand_list(SearchNode * node, CandList cand_list, int max_depth) {
//
////    std::cout << "Expanding list: " << name(cand_list) << "\n";
//
//    bool changes = false;
//    while (node != nullptr) {
////        std::cout << "Taking node from list\n";
//        changes = expand_node(node, cand_list, max_depth) || changes;
//        node = node->best_child;
//    }
//
//    return changes;
//}

bool expand_list(SearchNode * node, CandList cand_list, int max_depth) {

    if (node == nullptr) {
        return false;
    }

    bool changes_made = expand_list(node->best_child, cand_list, max_depth);
    changes_made = expand_node(node, cand_list, max_depth) || changes_made;

    update_score(node);

    return changes_made;
}

/**
 * For each node on the path from the root to the best node (ie following best_child pointers),
 * this method expands a new node for each candidate in the given list. Scores are then updated.
 * If given recursive=true, then it will also do this for each new node expanded (this option may
 * cause endless recursion - be careful).
 */
bool expand_node(SearchNode * node, CandList cand_list, int max_depth) {
    check_abort();

    if (max_depth == 0) {
        return false;
    }

//    std::cout << "Expanding node: " << name(cand_list) << "\n";

//    std::cout << "Critical: " << node->cand_set.critical.size() << "\n";
//    std::cout << "Medial: " << node->cand_set.medial.size() << "\n";
//    std::cout << "Final: " << node->cand_set.final.size() << "\n";
//    std::cout << "Legal: " << node->cand_set.legal.size() << "\n";

    // check for mate
    if (node->gs->has_been_mated) {
        node->score = node->gs->board.get_white()
                      ? white_has_been_mated()
                      : black_has_been_mated();
        node->best_child = nullptr;
        return false;
    }

    const std::vector<Move> & list = node->cand_set.get_list(cand_list);

    if (list.empty()) {
        return false;
    }

    // after this point, we know that changes will be made

    if (node->num_children == 0) {
        // set up the node's children pointers (allocate all)
        node->children = new SearchNode *[node->cand_set.size()];
    }

    // create child for each move in the list
    int c = node->num_children;
    for (int i = 0; i < list.size(); ++i) {
        node->children[c + i] = new_node(*node->gs, list[i]);
        node->children[c + i]->score = heur(*node->children[c + i]->gs);
        node->children[c + i]->cand_set = cands(*node->children[c + i]->gs);
        expand_node(node->children[c + i], cand_list, max_depth - 1);
    }
    node->num_children += list.size();
    node->cand_set.clear_list(cand_list);

    update_score(node);

    return true;
}
