#include <iostream>
#include <chrono>

#include "search.h"
#include "game.h"
#include "search_tools.h"
#include "../game/gamestate.hpp"

/**
 * Extends the CandSet of the given node to include legal moves. It removes from this list
 * all legal moves which (i) are already present in another list (ii) belong to an existing
 * child of this node.
 */
void add_legal_moves(SearchNode * node) {

    std::vector<Move> legals = legal_moves(node->gs->board);
    if (legals.empty()) {
        if (node->gs->in_check) {
            node->gs->game_over = true;
            node->terminal = true;
            node->score =
                node->gs->board.get_white()
                    ? white_has_been_mated()
                    : black_has_been_mated();
        } else {
            node->gs->game_over = true;
            node->terminal = true;
            node->score = zero();
        }
    }

    std::vector<Move> approved;
    approved.reserve(legals.size());

    for (int i = 0; i < legals.size(); ++i) {

        // check in children
        bool already_created = false;
        for (int j = 0; j < node->children.size(); ++j) {
            if (equal_exactly(node->children[j]->move, legals[i])) {
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
bool deepen(SearchNode * node, CandList cand_list, int depth, Observer & obs, bool burst=false) {

    // exit conditions
    check_abort();
    if (node->gs->game_over) {
        update_terminal(node, obs);
        return false;
    }

    if (depth == 0) {

        if (burst) {
            return false;
        }

        if (quiess(*node->gs) >= __engine_params::QUIESS_THRESHOLD) {
            obs.register_event(node, BEGIN_BURST);
            return deepen(node, CRITICAL, __engine_params::BURST_DEPTH, obs, true);
        } else {
            return false;
        }
    }

    if (burst && quiess(*node->gs) < __engine_params::QUIESS_THRESHOLD) {
        obs.register_event(node, DEBURST);
        return false;
    }

    obs.open_event(node, burst ? BURST_DEEPEN : DEEPEN, &cand_list);

    // no early exits: this counts as a visit unless in a burst (bursts
    // will only extend CRITICAL, so it doesn't make sense to count
    // higher than that).
    if (!burst || node->visit_count <= __engine_params::CRITICAL_THRESHOLD) {
        ++node->visit_count;
    }

    // fetch list to extend
    if (cand_list == CRITICAL) { node->cand_set->order_list(cand_list); }
    const std::vector<Move> & list = node->cand_set->get_list(cand_list);

    if (list.empty()) {
        // even if the list is empty, we still need to recurse up to the given depth
        bool changes = false;
        for (int i = 0; i < node->children.size(); ++i) {
            changes = deepen(node->children[i], cand_list, depth - 1, obs, burst)
                        || changes;
        }
        update_score(node);
        update_terminal(node, obs);
        obs.close_event(node, burst ? BURST_DEEPEN : DEEPEN, &cand_list, 1);
        return changes;
    }

    // create child for each move in the list (appending)
    int c = node->children.size();
    for (int i = 0; i < list.size(); ++i) {
        SearchNode * child = new_node(*node->gs, list[i]);
        child->cand_set = cands(*child->gs, child->cand_set);
        if (child->gs->game_over) {
            child->score =
                child->gs->board.get_white()
                    ? white_has_been_mated()
                    : black_has_been_mated();
        } else {
            child->score = heur(*child->gs);
        }
        node->children.push_back(child);
    }
    node->cand_set->clear_list(cand_list);

    // recurse as appropriate
    bool changes = (node->children.size() != c);
    for (int i = 0; i < node->children.size(); ++i) {
        changes = deepen(node->children[i], cand_list, depth - 1, obs, burst)
                    || changes;
    }

    update_score(node);
    update_terminal(node, obs);
    obs.close_event(node, burst ? BURST_DEEPEN : DEEPEN, &cand_list, 2);
    return changes;
}

/**
 * Visits a node. Depending on how many times the node in question has been visited,
 * a different candidate list will be expanded. In many cases, nothing will change
 * about the node except to update its score.
 * Returns true if new nodes were deepened and false otherwise.
 */
bool visit_node(SearchNode * node, Observer & obs) {

    if (!node || node->gs->game_over || node->terminal) {
        return false;
    }

    obs.open_event(node, VISIT);

    bool result;

    switch (node->visit_count) {
        case __engine_params::CRITICAL_THRESHOLD:
            result = deepen(node, CRITICAL, __engine_params::CRITICAL_DEPTH, obs);
            obs.close_event(node, VISIT, nullptr, 1);
            return result;
        case __engine_params::MEDIAL_THRESHOLD:
            result = deepen(node, MEDIAL, __engine_params::MEDIAL_DEPTH, obs);
            obs.close_event(node, VISIT, nullptr, 2);
            return result;
        case __engine_params::FINAL_THRESHOLD:
            result = deepen(node, FINAL, __engine_params::FINAL_DEPTH, obs);
            obs.close_event(node, VISIT, nullptr, 3);
            return result;
        case __engine_params::LEGAL_THRESHOLD:
            if (node->cand_set->legal.empty()) {
                add_legal_moves(node);
                if (node->terminal) {
                    // stalemate could have been detected
                    obs.close_event(node, VISIT, nullptr, 4);
                    return true;
                }
            }
            result = deepen(node, LEGAL, __engine_params::LEGAL_DEPTH, obs);
            obs.close_event(node, VISIT, nullptr, 5);
            return result;
        default:
            ++node->visit_count;
            update_score(node);
            update_terminal(node, obs);
            obs.close_event(node, VISIT, nullptr, 6);
            return false;
    }

}

/**
 * Visits a node and forces the next unexplored list to be extended, excluding
 * legal moves. If that list is empty, it moves on to the next one, still
 * excluding legal moves.
 * Returns true if any new nodes were in fact extended.
 */
bool force_visit(SearchNode * node, Observer & obs) {

    if (!node || node->gs->game_over || node->terminal) {
        return false;
    }

    bool changes = false;

    obs.open_event(node, FORCE_VISIT);

    if (node->visit_count <= __engine_params::CRITICAL_THRESHOLD) {
        node->visit_count = __engine_params::CRITICAL_THRESHOLD;
        changes = deepen(node, CRITICAL, __engine_params::CRITICAL_DEPTH, obs);
    }

    if (!changes && node->visit_count < __engine_params::MEDIAL_THRESHOLD) {
        node->visit_count = __engine_params::MEDIAL_THRESHOLD;
        changes = deepen(node, MEDIAL, __engine_params::MEDIAL_DEPTH, obs);
    }

    if (!changes && node->visit_count < __engine_params::FINAL_THRESHOLD) {
        node->visit_count = __engine_params::FINAL_THRESHOLD;
        changes = deepen(node, FINAL, __engine_params::FINAL_DEPTH, obs);
    }

    if (!changes) {
        // there may have been changes to other nodes in the line being force-visited,
        // and if there were no changes here then deepen won't have called the updates.
        update_score(node);
        update_terminal(node, obs);
    }

    obs.close_event(node, FORCE_VISIT);
    return changes;
}

/**
 * Follows the best line from root to leaf, force-visiting each node in order
 * from the leaf back up to the root. The scores are updated after the extension,
 * so the line visited is that which is the best line in the initial position.
 * Returns true if any changes were made, false otherwise.
 */
bool force_visit_best_line(SearchNode * node, Observer & obs) {

    if (node == nullptr) { return false; }

    obs.open_event(node, FORCE_VISIT_LINE);
    bool changes = force_visit(node->best_child, obs);
    changes = force_visit(node, obs) || changes;

    obs.close_event(node, FORCE_VISIT_LINE);
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
bool visit_best_line(SearchNode * node, bool in_swing, Observer & obs) {

    if (node == nullptr) { return false; }

    Eval prior = node->score;
    obs.open_event(node, VISIT_LINE);

    // recurse first so that the line is visited bottom (deepest) first
    bool changes = visit_best_line(node->best_child, in_swing, obs);
    changes = visit_node(node, obs) || changes;

    if (!in_swing && is_swing(prior, node->score)) {
        changes = force_visit_best_line(node, obs) || changes;
    }

    obs.close_event(node, VISIT_LINE);
    return changes;

}

std::vector<Move> greedy_search(SearchNode * root, int cycles, Observer & obs) {

    if (root->score == zero()) {
        root->score = heur(*root->gs);
    }
    if (root->cand_set->empty()) {
        delete root->cand_set;
        root->cand_set = cands(*root->gs, new CandSet);
    }

    int i = 0;

//    auto start = std::chrono::high_resolution_clock::now();

    while (i++ < cycles || cycles < 0) {

        visit_best_line(root, false, obs);

//        auto stop = std::chrono::high_resolution_clock::now();
//        long duration = duration_cast<std::chrono::microseconds>(stop - start).count();
//        double seconds = ((double)duration) / 1000000.0;
//
//        std::cout << i << ": (" << ((double) node_count()) / seconds << ") ";
//        std::vector<SearchNode *> best_line = retrieve_best_line(root);
//        print_line(best_line);

        if (root->terminal || soft_exit_criteria(root)) {
            break;
        }
    }

    std::vector<SearchNode *> best_line = retrieve_trust_line(root);
//    print_line(best_line);

    std::vector<Move> moves;
    for (SearchNode * s : best_line) {
        if (s != root) {
            moves.push_back(s->move);
        }
    }

    std::string name = "stase_tree";
    // record_tree_in_file(name, root);

    return moves;
}

std::vector<Move> greedy_search(const std::string & fen, int cycles, Observer & obs) {

    // set up the root node
    Gamestate root_gs(fen);
    SearchNode root{
        &root_gs,
        cands(root_gs, new CandSet),
        heur(root_gs),
        MOVE_SENTINEL,
        false,
        {},
        nullptr,
        nullptr,
        0
    };

    return greedy_search(&root, cycles, obs);

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
