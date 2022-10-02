#include <iostream>
#include <chrono>

#include "search.h"
#include "game.h"
#include "search_tools.h"
#include "../game/gamestate.hpp"

/**
 * @return true if there are three identical positions on the path from the given node to the root.
 * Note that this is not sufficient to avoid threefold repetition: you also need to know about moves
 * from the whole game.
 */
bool threefold_rep(const SearchNode * node, const std::vector<Board> * board_history) {

    int count = 0;
    const SearchNode * current = node->parent;

    while (current) {
        if (current->board_hash == node->board_hash && node->gs->board.equivalent(current->gs->board)) {
            if (++count == 3) {
                return true;
            }
        }
        if (current->parent) {
            // check for pawn moves!
            Piece moved = current->parent->gs->board.get(current->move.from);
            if (type(moved) == PAWN) {
                return false;
            }
        }
        current = current->parent;
    }

    if (!board_history) { return false; }

    // we now search the board history
    for (int i = board_history->size() - 1; i >= 0; --i) {
        const Board & b = (*board_history)[i];
        if (board_hash(b) == node->board_hash && b.equivalent(node->gs->board)) {
            if (++count == 3) {
                return true;
            }
        }
        // TODO: somehow check for pawn moves or captures and abort here.
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
bool deepen(SearchNode * node, CandList cand_list, int depth, const std::vector<Board> * board_history, Observer & obs, bool burst=false) {

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

        if (node->visit_count <= __engine_params::CRITICAL_THRESHOLD
              && quiess(*node->gs) >= __engine_params::QUIESS_THRESHOLD) {
            obs.register_event(node, BEGIN_BURST);
            return deepen(node, CRITICAL, __engine_params::BURST_DEPTH, board_history, obs, true);
        } else {
            return false;
        }
    }

    if (burst) {
        if (node->visit_count > __engine_params::CRITICAL_THRESHOLD
            || quiess(*node->gs) < __engine_params::QUIESS_THRESHOLD
            || node->cand_set->critical.empty()) {
            obs.register_event(node, DEBURST);
            return false;
        }
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
            changes = deepen(node->children[i], cand_list, depth - 1, board_history, obs, burst)
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
        SearchNode * child = new_node(node, list[i]);
        child->cand_set = cands(*child->gs, child->cand_set);
        if (child->gs->game_over) {
            child->score =
                child->gs->board.get_white()
                    ? white_has_been_mated()
                    : black_has_been_mated();
        } else if (threefold_rep(child, board_history)) {
            child->score = zero();
            child->gs->game_over = true;
            obs.register_event(child, THREEFOLD_REP);
        } else {
            child->score = heur(*child->gs);
        }
        node->children.push_back(child);
    }
    node->cand_set->clear_list(cand_list);

    // recurse as appropriate
    bool changes = (node->children.size() != c);
    for (int i = 0; i < node->children.size(); ++i) {
        changes = deepen(node->children[i], cand_list, depth - 1, board_history, obs, burst)
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
void visit_node(SearchNode * node, const std::vector<Board> * board_history, Observer & obs) {

    if (!node || node->gs->game_over || node->terminal) {
        return;
    }

    obs.open_event(node, VISIT);

    switch (node->visit_count) {
        case __engine_params::CRITICAL_THRESHOLD:
            deepen(node, CRITICAL, __engine_params::CRITICAL_DEPTH, board_history, obs);
            obs.close_event(node, VISIT, nullptr, 1);
            return;
        case __engine_params::MEDIAL_THRESHOLD:
            deepen(node, MEDIAL, __engine_params::MEDIAL_DEPTH, board_history, obs);
            obs.close_event(node, VISIT, nullptr, 2);
            return;
        case __engine_params::FINAL_THRESHOLD:
            deepen(node, FINAL, __engine_params::FINAL_DEPTH, board_history, obs);
            obs.close_event(node, VISIT, nullptr, 3);
            return;
        case __engine_params::LEGAL_THRESHOLD:
            if (node->cand_set->legal.empty()) {
                add_legal_moves(node);
                if (node->terminal) {
                    // stalemate could have been detected
                    obs.close_event(node, VISIT, nullptr, 4);
                    return;
                }
            }
            deepen(node, LEGAL, __engine_params::LEGAL_DEPTH, board_history, obs);
            obs.close_event(node, VISIT, nullptr, 5);
            return;
        default:
            ++node->visit_count;
            update_score(node);
            update_terminal(node, obs);
            obs.close_event(node, VISIT, nullptr, 6);
            return;
    }

}

/**
 * Visits a node and forces the next unexplored list to be extended, excluding
 * legal moves. If that list is empty, it moves on to the next one, still
 * excluding legal moves.
 * Returns true if any new nodes were in fact extended.
 */
bool force_visit(SearchNode * node, const std::vector<Board> * board_history, Observer & obs) {

    if (!node || node->gs->game_over || node->terminal) {
        return false;
    }

    bool changes = false;

    obs.open_event(node, FORCE_VISIT);

    if (node->visit_count <= __engine_params::CRITICAL_THRESHOLD) {
        node->visit_count = __engine_params::CRITICAL_THRESHOLD;
        changes = deepen(node, CRITICAL, __engine_params::CRITICAL_DEPTH, board_history, obs);
    }

    if (!changes && node->visit_count < __engine_params::MEDIAL_THRESHOLD) {
        node->visit_count = __engine_params::MEDIAL_THRESHOLD;
        changes = deepen(node, MEDIAL, __engine_params::MEDIAL_DEPTH, board_history, obs);
    }

    if (!changes && node->visit_count < __engine_params::FINAL_THRESHOLD) {
        node->visit_count = __engine_params::FINAL_THRESHOLD;
        changes = deepen(node, FINAL, __engine_params::FINAL_DEPTH, board_history, obs);
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
bool force_visit_best_line(SearchNode * node, const std::vector<Board> * board_history, Observer & obs) {

    if (node == nullptr) { return false; }

    obs.open_event(node, FORCE_VISIT_LINE);
    bool changes = force_visit(node->best_child, board_history, obs);
    changes = force_visit(node, board_history, obs) || changes;

    obs.close_event(node, FORCE_VISIT_LINE);
    return changes;
}

/**
 * Follows the best line from root to leaf, visiting each node in reverse (first
 * the leaf node, then back up to the root. If a swing is detected at any point,
 * then the path from the leaf back up to the current node is visited again, at
 * most once.
 * Returns true if any descendant of the given node caused (and executed) a swing,
 * and false otherwise.
 */
bool visit_best_line(SearchNode * node, const std::vector<Board> * board_history, Observer & obs) {

    if (node == nullptr) { return false; }

    Eval prior = node->score;
    obs.open_event(node, VISIT_LINE);

    // recurse on the best child
    bool has_swung = visit_best_line(node->best_child, board_history, obs);

    // recurse on very-nearly-equal siblings
    int extra_siblings_visited = 0;
    for (int i = 0; i < node->children.size(); ++i) {
        if (node->children[i] == node->best_child) { continue; }
        if (millipawn_diff(node->best_child->score, node->children[i]->score) <= __engine_params::EVALS_EQUAL_THRESHOLD) {
            has_swung = visit_best_line(node->children[i], board_history, obs) || has_swung;
            if (++extra_siblings_visited == __engine_params::MAX_EXTRA_SIBLINGS) {
                break;
            }
        }
    }

    // after the below recursion, visit the current node itself
    visit_node(node, board_history, obs);

    if (!has_swung && is_swing(prior, node->score)) {
        obs.register_event(node, SWING);
        force_visit_best_line(node, board_history, obs);
        obs.close_event(node, VISIT_LINE, nullptr, 1);
        return true;
    }

    obs.close_event(node, VISIT_LINE, nullptr, 2);
    return has_swung;
}

std::vector<Move> greedy_search(SearchNode * root, int cycles, const std::vector<Board> * board_history, Observer & obs) {

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

        visit_best_line(root, board_history, obs);

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
    SearchNode root(&root_gs, cands(root_gs, new CandSet), heur(root_gs));

    return greedy_search(&root, cycles, nullptr, obs);

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
