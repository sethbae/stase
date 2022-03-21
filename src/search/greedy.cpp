#include <iostream>
#include <chrono>

#include "search.h"
#include "game.h"
#include "search_tools.h"
#include "../game/gamestate.hpp"

namespace __constants {

    /**
     * These constants control certain behaviours of the search algorithm.
     * THRESHOLDs determine at what visit count a set of candidates is expanded.
     * DEPTHs determine how deep a tree will be extended at once.
     * QUIESS_THRESHOLD determines the value at which positions become non-quiescent.
     */

    const int CRITICAL_THRESHOLD = 0;
    const int MEDIAL_THRESHOLD = 2;
    const int FINAL_THRESHOLD = 8;
    const int LEGAL_THRESHOLD = 256;

    const float QUIESS_THRESHOLD = 2.0f;

    const int BURST_DEPTH = 5;
    const int CRITICAL_DEPTH = 2;
    const int MEDIAL_DEPTH = 1;
    const int FINAL_DEPTH = 1;
    const int LEGAL_DEPTH = 1;

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

        // check in children
        bool already_created = false;
        for (int j = 0; j < node->children.size(); ++j) {
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
bool deepen(SearchNode * node, CandList cand_list, int depth, Observer & obs, bool burst=false) {

    // exit conditions
    check_abort();
    if (node->gs->has_been_mated) { return false; }

    if (depth == 0) {

        if (burst) {
            return false;
        }

        if (quiess(*node->gs) >= __constants::QUIESS_THRESHOLD) {
            obs.register_event(node, BEGIN_BURST);
            return deepen(node, CRITICAL, __constants::BURST_DEPTH, obs, true);
        } else {
            return false;
        }
    }

    if (burst && quiess(*node->gs) < __constants::QUIESS_THRESHOLD) {
        return false;
    }

    obs.open_event(node, burst ? BURST_DEEPEN : DEEPEN, &cand_list);

    // no early exits: this counts as a visit unless in a burst (bursts
    // will only extend CRITICAL, so it doesn't make sense to count
    // higher than that).
    if (!burst || node->visit_count <= __constants::CRITICAL_THRESHOLD) {
        ++node->visit_count;
    }

    // fetch list to extend
    const std::vector<Move> & list = node->cand_set->get_list(cand_list);
    if (list.empty()) {
        // we still need to recurse up to the given depth
        bool changes = false;
        for (int i = 0; i < node->children.size(); ++i) {
            changes = deepen(node->children[i], cand_list, depth - 1, obs, burst)
                        || changes;
        }
        update_score(node);
        obs.close_event(node, burst ? BURST_DEEPEN : DEEPEN, &cand_list);
        return changes;
    }

    // create child for each move in the list (appending)
    int c = node->children.size();
    for (int i = 0; i < list.size(); ++i) {
        SearchNode * child = new_node(*node->gs, list[i]);
        child->cand_set = cands(*child->gs, new CandSet);
        if (child->gs->has_been_mated) {
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
    obs.close_event(node, burst ? BURST_DEEPEN : DEEPEN, &cand_list);
    return changes;
}

/**
 * Visits a node. Depending on how many times the node in question has been visited,
 * a different candidate list will be expanded. In many cases, nothing will change
 * about the node except to update its score.
 * Returns true if new nodes were deepened and false otherwise.
 */
bool visit_node(SearchNode * node, Observer & obs) {

    if (node->gs->has_been_mated) {
        return false;
    }

    obs.open_event(node, VISIT);

    bool result;

    switch (node->visit_count) {
        case __constants::CRITICAL_THRESHOLD:
            result = deepen(node, CRITICAL, __constants::CRITICAL_DEPTH, obs);
            obs.close_event(node, VISIT);
            return result;
        case __constants::MEDIAL_THRESHOLD:
            result = deepen(node, MEDIAL, __constants::MEDIAL_DEPTH, obs);
            obs.close_event(node, VISIT);
            return result;
        case __constants::FINAL_THRESHOLD:
            result = deepen(node, FINAL, __constants::FINAL_DEPTH, obs);
            obs.close_event(node, VISIT);
            return result;
        case __constants::LEGAL_THRESHOLD:
            if (node->cand_set->legal.empty()) {
                add_legal_moves(node);
            }
            result = deepen(node, LEGAL, __constants::LEGAL_DEPTH, obs);
            obs.close_event(node, VISIT);
            return result;
        default:
            ++node->visit_count;
            update_score(node);
            obs.close_event(node, VISIT);
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

    bool changes = false;

    obs.open_event(node, FORCE_VISIT);

    if (node->visit_count <= __constants::CRITICAL_THRESHOLD) {
        node->visit_count = __constants::CRITICAL_THRESHOLD;
        changes = deepen(node, CRITICAL, __constants::CRITICAL_DEPTH, obs);
    }

    if (!changes && node->visit_count < __constants::MEDIAL_THRESHOLD) {
        node->visit_count = __constants::MEDIAL_THRESHOLD;
        changes = deepen(node, MEDIAL, __constants::MEDIAL_DEPTH, obs);
    }

    if (!changes && node->visit_count < __constants::FINAL_THRESHOLD) {
        node->visit_count = __constants::FINAL_THRESHOLD;
        changes = deepen(node, FINAL, __constants::FINAL_DEPTH, obs);
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

    auto start = std::chrono::high_resolution_clock::now();

    while (i++ < cycles || cycles < 0) {
        visit_best_line(root, false, obs);

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
