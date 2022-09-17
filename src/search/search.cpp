#include "game.h"
#include "search.h"
#include "search_tools.h"
#include "../game/gamestate.hpp"
#include "../game/eval.hpp"
#include "metrics.h"
#include "observers/observers.hpp"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;
#include <fstream>
using std::ofstream;
#include <ostream>
using std::ostream;

Observer DEFAULT_OBSERVER;

/**
 * Recursively counts the number of nodes in the subtree rooted at the given node.
 */
int subtree_size(SearchNode * node) {

    if (node == nullptr) { return 0; }

    int size = 1;
    for (int i = 0; i < node->children.size(); ++i) {
        size += subtree_size(node->children[i]);
    }
    return size;
}

/**
 * Returns the depth of the subtree rooted at the given node.
 */
int subtree_depth(SearchNode *node) {

    if (node == nullptr || node->children.size() == 0) { return 0; }

    int max = 0;
    for (int i = 0; i < node->children.size(); ++i) {
        int depth = subtree_depth(node->children[i]);
        if (depth > max) {
            max = depth;
        }
    }
    return max + 1;
}

/**
 * Prints out a single line of text to cout, for example:
 * [3.086] Ka1 Rf8 Ra4 Be5 Rxd4
 *
 * @param line the nodes in the line
 */
void print_line(std::vector<SearchNode *> & line) {

    if (line.empty()) {
        cout << "[empty line]\n";
        return;
    }

    cout << "[" << etos(line[0]->score) << "] ";
    for (int i = 1; i < line.size(); ++i) {
        cout << mtos(line[i-1]->gs->board, line[i]->move) << " ";
    }
    cout << "\n";
}

/**
 * Writes a summary of the given node to the output. This acts on a single node
 * only and does not recurse.
 */
void write_to_file(SearchNode *node, ostream & output) {

    /*
     * Format is:
     * some hashtags ########
     * SearchNode title line (with "created by" move)
     * board with conf
     * score
     * children title
     * list of children and names
     */
    std::vector<SearchNode *> best_line = retrieve_best_line(node);
    std::vector<SearchNode *> trust_line = retrieve_trust_line(node);

    output << "######################\n";

    // SearchNode title line
    output << "SearchNode at " << node
           << " (created by " << mtos(node->gs->board, node->move) << ")\n";

    wr_board_conf(node->gs->board, output);

    output << "\nScore: " << etos(node->score) << "\n";
    output << "Trust score: " << etos(trust_line[trust_line.size() - 1]->score) << "\n";
    output << "Game over? " << (node->gs->game_over ? "true\n" : "false\n");
    output << "Is terminal? " << (node->terminal ? "true\n" : "false\n");
    output << "Is in check? " << (node->gs->in_check ? "true\n" : "false\n");
    output << "Visit count: " << node->visit_count << "\n\n";

    if (node->children.size() == 0) {
        output << "Has no children.\n";
    } else {
        output << "Children:\n";
        output << node->children.size() << " children\n";

        // list of children and names
        for (int i = 0; i < node->children.size(); ++i) {
            output << "Child " << i << ": " << node->children[i]
                   << " (" << mtos(node->gs->board, node->children[i]->move)
                   << ") (" << etos(node->children[i]->score)
                   << ") (" << etos(trust_score(node->children[i], node->gs->board.get_white()))
                   << ") (" << subtree_depth(node->children[i]) << ")\n";
        }
    }

    if (node->best_child == nullptr) {
        output << "Best child: nullptr\n";
    } else {
        output << "Best child: " << node->best_child << "(" << etos(node->best_child->score) << ")\n";
    }
    if (node->best_trust_child == nullptr) {
        output << "Trust child: nullptr\n";
    } else {
        output << "Trust child: " << node->best_trust_child << "(" << etos(node->best_trust_child->score) << ")\n";
    }

    output << "Candidates:\n";
    print_cand_set(*node->gs, *node->cand_set, output);

    output << "Best line from here: (" << etos(node->score) << ")";
    for (int i = 1; i < best_line.size(); ++i) {
        output << " " << mtos(best_line[i-1]->gs->board, best_line[i]->move);
    }
    output << "\n";

    output << "Trust line from here: (" << etos(trust_line[trust_line.size() - 1]->score) << ")";
    for (int i = 1; i < trust_line.size(); ++i) {
        output << " " << mtos(trust_line[i-1]->gs->board, trust_line[i]->move);
    }

    output << "\n";

    return;

}

/**
 * Writes the given node to the file. Then, calls this function recursively on each
 * child of the node. A pre-order traversal is used.
 */
void write_to_file_recursively(SearchNode *node, ostream & output) {
    write_to_file(node, output);
    for (int i = 0; i < node->children.size(); ++i) {
        write_to_file_recursively(node->children[i], output);
    }
}

void record_tree_in_file(const std::string & filename, SearchNode * root) {
    ofstream file;
    file.open(filename, std::ios::out);

    file << "This file contains engine analysis of the following position\n";

    wr_board_conf(root->gs->board, file);

    file << "\nHere are the nodes:\n";

    write_to_file_recursively(root, file);
    file.close();
}

/**
 * Allocates a new gamestate according to the previous state and the move given. The new
 * gamestate will represent the position after the given move has been played.
 */
SearchNode *new_node(const Gamestate & gs, Move m) {
    register_new_node();
    Gamestate * new_gs = new Gamestate(gs, m);
    return new SearchNode{
        new_gs,
        new CandSet,
        zero(),
        m,
        false,
        board_hash(new_gs),
        {},
        nullptr,
        nullptr,
        0
    };
}

/**
 * Checks the children of the node and sets the score and best_child pointers
 * accordingly.
 */
void update_score(SearchNode * node) {

    check_abort();
    if (node->children.size() == 0) { return; }

    // find the best score among children
    node->score = node->children[0]->score;
    node->best_child = node->children[0];

    for (int i = 1; i < node->children.size(); ++i) {
        Eval score = node->children[i]->score;
        if (node->gs->board.get_white() && score > node->score) {
            node->score = score;
            node->best_child = node->children[i];
        } else if (!node->gs->board.get_white() && score < node->score) {
            node->score = score;
            node->best_child = node->children[i];
        }
    }

    // if the score is mate, then we need to make it mate in one more move
    if (is_mate(node->score)) {
        if (white_is_mated(node->score) && !node->gs->board.get_white()) {
            node->score = mate_in_one_more(node->score);
        } else if (black_is_mated(node->score) && node->gs->board.get_white()) {
            node->score = mate_in_one_more(node->score);
        }
    }

    // now find the most trusted score among children
    Eval best_trust_score = trust_score(node->children[0], node->gs->board.get_white());
    node->best_trust_child = node->children[0];

    for (int i = 1; i < node->children.size(); ++i) {
        Eval score = trust_score(node->children[i], node->gs->board.get_white());
        if (node->gs->board.get_white() && score > best_trust_score) {
            best_trust_score = score;
            node->best_trust_child = node->children[i];
        } else if (!node->gs->board.get_white() && score < best_trust_score) {
            best_trust_score = score;
            node->best_trust_child = node->children[i];
        }
    }
}

/**
 * Given a pointer to the root of a tree, retrieves the best line of play as indicated
 * by the scores on the nodes. This includes the given root, as the first element.
 */
std::vector<SearchNode *> retrieve_best_line(SearchNode * root) {
    std::vector<SearchNode *> line;
    SearchNode * current = root;

    while (current != nullptr) {
        line.push_back(current);
        update_score(current);
        current = current->best_child;
    }

    return line;
}

/**
 * Given a pointer to the root of a tree, retrieves the most trustworthy line of play
 * as indicated by the trust_score for each node. This includes the given root, as the first element.
 */
std::vector<SearchNode *> retrieve_trust_line(SearchNode * root) {
    std::vector<SearchNode *> line;
    SearchNode * current = root;

    while (current != nullptr) {
        line.push_back(current);
        update_score(current);
        current = current->best_trust_child;
    }

    return line;
}

/**
 * Deletes completely the tree or subtree of which the given node is the root.
 */
void delete_tree(SearchNode * node) {

    if (!node) { return; }

    for (int i = 0; i < node->children.size(); ++i) {
        delete_tree(node->children[i]);
    }
    delete node->gs;
    delete node->cand_set;
    delete node;
}
