#include <iostream>
#include "search.h"
#include "game.h"
#include "search_tools.h"

bool expand_list(SearchNode *, CandList, bool);
bool expand_node(SearchNode *, CandList, bool);

std::vector<Move> greedy_search(const std::string & fen) {

    // set up the root node
    Gamestate root_gs(fen_to_board(fen));
    SearchNode root{
        &root_gs,
        cands(root_gs),
        zero(),
        MOVE_SENTINEL,
        0,
        nullptr,
        nullptr
    };

    int i = 0;

    while (i++ < 3) {

        if (expand_list(&root, CRITICAL, true)) { continue; }

        if (expand_list(&root, MEDIAL, false)) { continue; }

        if (expand_list(&root, FINAL, false)) { continue; }

        if (expand_list(&root, LEGAL, false)) { continue; }

    }

    std::vector<SearchNode *> best_line = retrieve_best_line(&root);
    print_line(best_line);

    std::vector<Move> moves;
    for (SearchNode * s : best_line) {
        if (s != &root) {
            moves.push_back(s->move);
        }
    }

    return moves;
}

/**
 * Follows the best child pointers from the given node until a leaf is reached. For each
 * node, it expands it according to the list and recursive flag given.
 */
bool expand_list(SearchNode * node, CandList cand_list, bool recursive) {

    std::cout << "Expanding list: " << name(cand_list) << "\n";

    if (recursive) {
        // if recursive, there's no need for us to work down the path: that will occur naturally.
        return expand_node(node, cand_list, recursive);
    }

    std::cout << "here\n";

    bool changes = false;
    while (node != nullptr) {
        changes = expand_list(node, cand_list, recursive) || changes;
        node = node->best_child;
    }

    return changes;
}

/**
 * For each node on the path from the root to the best node (ie following best_child pointers),
 * this method expands a new node for each candidate in the given list. Scores are then updated.
 * If given recursive=true, then it will also do this for each new node expanded (this option may
 * cause endless recursion - be careful).
 */
bool expand_node(SearchNode * node, CandList cand_list, bool recursive) {
    check_abort();

    std::cout << "Expanding node: " << name(cand_list) << "\n";

    bool changes = false;

    // fetch cands
    if (!node->gs->has_been_mated && node->cand_set.empty()) {
        node->cand_set = cands(*node->gs);
        changes = true;
    }

    // check for mate
    if (node->gs->has_been_mated) {
        node->score = node->gs->board.get_white()
                      ? white_has_been_mated()
                      : black_has_been_mated();
        node->best_child = nullptr;
        return changes;
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
    for (int i = node->num_children; i < node->num_children + list.size(); ++i) {
        node->children[i] = new_node(*node->gs, list[i]);
        if (recursive) {
            expand_node(node->children[i], cand_list, recursive);
        }
    }
    node->num_children += list.size();
    node->cand_set.clear_list(cand_list);

    update_score(node);

    return true;
}
