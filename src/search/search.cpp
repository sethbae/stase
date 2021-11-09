#include "game.h"
#include "search.h"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;
#include <fstream>
using std::ofstream;
#include <ostream>
using std::ostream;

// hacky way to count the number of nodes we create
int COUNT = 0;

int node_count() {
    return COUNT;
}

void reset_node_count() {
    COUNT = 0;
}

// recursively count the number of nodes in the subtree rooted at the given node
int subtree_size(SearchNode *node) {

    if (node == nullptr) {
        return 0;
    }

    int size = 1;

    for (int i = 0; i < node->num_children; ++i)
        size += subtree_size(node->children[i]);

    return size;

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
        cout << mtos(line[i]->gs->board, line[i]->move) << " ";
    }
    cout << "\n";
}

/**
 * Writes the given tree or subtree to file. May produce extremely large output!
 * A pre-order traversal is used.
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

    output << "######################\n";

    // SearchNode title line
    output << "SearchNode at " << node
           << " (created by " << mtos(node->gs->board, node->move) << ")\n";

    wr_board_conf(node->gs->board, output);

    output << "\nScore: " << etos(node->score) << "\n";
    output << "Has been mated? " << (node->gs->has_been_mated ? "true\n\n" : "false\n\n");

    if (node->num_children == 0) {
        output << "Has no children.\n";
    } else {
        output << "Children:\n";

        // list of children and names
        for (int i = 0; i < node->num_children; ++i) {
            output << "Child " << i << ": " << node->children[i]
                   << " (" << mtos(node->children[i]->gs->board, node->children[i]->move)
                   << ") (" << etos(node->children[i]->score) << ")\n";
        }
    }

    if (node->best_child == nullptr) {
        output << "Best child: nullptr\n";
    } else {
        output << "Best child: " << node->best_child << "\n";
    }

    output << "\n";

    // recurse for each child
    for (int i = 0; i < node->num_children; ++i) {
        write_to_file(node->children[i], output);
    }

    return;

}

void record_tree_in_file(const std::string & filename, SearchNode * root) {

    ofstream file;
    file.open(filename, std::ios::out);

    file << "This file contains engine analysis of the following position\n";

    wr_board_conf(root->gs->board, file);

    file << "\nHere are the nodes:\n";

    write_to_file(root, file);
    file.close();

}

/**
 * Allocates a new gamestate according to the previous state and the move given. The new
 * gamestate will represent the position after the given move has been played.
 */
SearchNode *new_node(const Gamestate & gs, Move m) {

    COUNT++;

    Gamestate *new_gs = new Gamestate;
    new_gs->board = gs.board.successor(m);

    SearchNode *new_node = new SearchNode;
    new_node->gs = new_gs;
    new_node->score = (Eval) 0;
    new_node->move = m;
    new_node->num_children = 0;
    new_node->children = nullptr;

    return new_node;

}

/**
 * Checks the children of the node and sets the score and best_child pointers
 * accordingly.
 */
void update_score(SearchNode * node) {

    if (node->num_children == 0) {
        node->best_child = nullptr;
        return;
    }

    // find the best score among children
    node->score = node->children[0]->score;
    node->best_child = node->children[0];

    for (int i = 1; i < node->num_children; ++i) {

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
        node->score = mate_in_one_more(node->score);
    }
}

/**
 * Deepens the tree or sub-tree pointed to. For each terminal node, this retrieves candidates
 * and extends accordingly. The heuristic evaluation is called on each node and the scores of
 * all nodes are updated accordingly.
 */
void deepen_tree(SearchNode * node, int alpha, int beta) {

    bool white = node->gs->board.get_white();

    if (node->num_children == 0) {

        // get candidate moves and initialise the score counter
        vector<Move> moves = cands(*node->gs);

        if (node->gs->has_been_mated) {
            node->score = node->gs->board.get_white()
                    ? white_has_been_mated()
                    : black_has_been_mated();
            node->best_child = nullptr;
            return;
        }

        if (moves.empty()) {
            return;
        }

        // set up the node's children pointers
        node->children = new SearchNode *[moves.size()];
        node->num_children = moves.size();

        for (int i = 0; i < moves.size(); ++i) {
            // create a new child and perform the heuristic eval
            node->children[i] = new_node(*(node->gs), moves[i]);
            node->children[i]->score = heur(*node->children[i]->gs);
        }

        update_score(node);

        return;

    } else {

        // deepen tree on each child recursively, updating a/b
        for (int i = 0; i < node->num_children && alpha < beta; ++i) {

            deepen_tree(node->children[i], alpha, beta);

            Eval score = node->children[i]->score;
            if (white && score > alpha) {
                alpha = score;
            } else if (!white && score < beta) {
                beta = score;
            }
        }

        update_score(node);
    }
}

void deepen_tree(SearchNode * root) {
    deepen_tree(root, white_has_been_mated(), black_has_been_mated());
}

/**
 * Given a pointer to the root of a tree, retrieves the best line of play as indicated
 * by the scores on the nodes.
 */
std::vector<SearchNode *> retrieve_best_line(SearchNode * root) {
    std::vector<SearchNode *> line{root};
    SearchNode * current = root;

    while (current->best_child) {
        line.push_back(current->best_child);
        current = current->best_child;
    }

    return line;
}

/**
 * Begins a search from the given FEN, performing repeated DFS to increasing depths.
 * Returns a vector of moves which is the best line of play.
 */
std::vector<Move> iterative_deepening_search(const std::string & fen, int max_depth) {

    // initialise with root only
    Gamestate root_gs(fen_to_board(fen));
    SearchNode root = { &root_gs, (Eval) 0, MOVE_SENTINEL, 0, nullptr };

    auto start = std::chrono::high_resolution_clock::now();

    int d = 0;
    while (++d <= max_depth) {
        deepen_tree(&root);

        auto stop = std::chrono::high_resolution_clock::now();
        long duration = duration_cast<std::chrono::microseconds>(stop - start).count();
        double seconds = ((double)duration) / 1000000.0;

        cout << d << ": (" << ((double) COUNT) / seconds << ") ";
        std::vector<SearchNode *> best_line = retrieve_best_line(&root);
        print_line(best_line);
    }

    std::vector<SearchNode *> best_line = retrieve_best_line(&root);
    print_line(best_line);

    std::vector<Move> moves;
    for (SearchNode * s : best_line) {
        if (s != &root) {
            moves.push_back(s->move);
        }
    }

    record_tree_in_file("stase_tree", &root);

    return moves;
}

/**
 * Extends the search tree indefinitely from the root. This method will never return and
 * is intended to be used in a multi-threaded context only.
 */
void search_indefinite(SearchNode * root) {
    while (true) {
        deepen_tree(root);
    }
}

/**
 * Deletes completely the tree or subtree of which the given node is the root.
 */
void delete_tree(SearchNode * node) {

    if (!node) { return; }

    for (int i = 0; i < node->num_children; ++i) {
        delete_tree(node->children[i]);
    }
    delete node;
}
