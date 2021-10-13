#include "game.h"
#include "search.h"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;
#include <functional>
#include <fstream>
using std::ofstream;
#include <ostream>
using std::ostream;

int COUNT = 0;

/**
 * Prints out a single line of text to cout, for example:
 * [3.086] Ka1 Rf8 Ra4 Be5 Rxd4
 *
 * @param line the nodes in the line
 */
void print_line(std::vector<SearchNode *> & line) {
    cout << "[" << human_eval(line[0]->score) << "] ";
    for (SearchNode * s : line) {
        cout << mtos(s->gs->board, s->move) << " ";
    }
    cout << "\n";
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
 * Deepens the tree or sub-tree pointed to. For each terminal node, this retrieves candidates
 * and extends accordingly. The heuristic evaluation is called on each node and the scores of
 * all nodes are updated accordingly.
 */
void deepen_tree(SearchNode * node) {

    bool white = node->gs->board.get_white();

    if (node->num_children == 0) {

        // get candidate moves and initialise the score counter
        vector<Move> moves = cands(*node->gs);
        Eval best_score = white ? white_has_been_mated() : black_has_been_mated();

        // set up the node's children pointers
        node->children = new SearchNode *[moves.size()];
        node->num_children = moves.size();

        for (int i = 0; i < moves.size(); ++i) {

            // create a new child and perform the heuristic eval
            node->children[i] = new_node(*(node->gs), moves[i]);
            Eval score = heur(*node->children[i]->gs);
            node->children[i]->score = score;

            // update the running totals
            if (white && score > best_score) {
                best_score = score;
            } else if (!white && score < best_score) {
                best_score = score;
            }

        }

        // set the best score and we're done!
        node->score = best_score;
        return;

    } else {

        // reset the score
        node->score = white ? white_has_been_mated() : black_has_been_mated();

        // deepen tree on each child recursively, updating the score as we go
        for (int i = 0; i < node->num_children; ++i) {
            deepen_tree(node->children[i]);
            if (white && node->children[i]->score > node->score) {
                node->score = node->children[i]->score;
            } else if (!white && node->children[i]->score < node->score) {
                node->score = node->children[i]->score;
            }
        }

    }
}

/**
 * Given a pointer to the root of a tree, retrieves the best line of play as indicated
 * by the scores on the nodes.
 */
std::vector<SearchNode *> retrieve_best_line(SearchNode * root) {
    std::vector<SearchNode *> line;
    SearchNode * current = root;

    while (current->num_children) {

        bool found = false;
        for (int i = 0; i < current->num_children; ++i) {
            if (current->children[i]->score == current->score) {
                line.push_back(current->children[i]);
                current = current->children[i];
                found = true;
                break;
            }
        }

        if (!found) { return line; }
    }

    return line;
}

/**
 * Begins a search from the given FEN, performing repeated DFS to increasing depths.
 * Returns a vector of moves which is the best line of play.
 */
std::vector<Move> iterative_deepening_search(const std::string & fen, int max_depth) {

    // initialise with root only
    const Gamestate root_gs(fen_to_board(fen));
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

    return moves;
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

void readable_printout(vector<SearchNode*> & nodes, ostream & output) {

    auto itr = nodes.rbegin();

    wr_board_conf((*itr)->gs->board, output);
    output << "\nBest line: ";
    itr++;

    while (itr != nodes.rend()) {

        SearchNode *node = *itr;

        output << mtos(node->gs->board, node->move) << " ";
        //pr_board(node->gs->board);

        itr++;

    }

    output << "\n";

    output << "Evaluation: " << nodes[nodes.size()-1]->score << "\n";
    output.flush();

}

// write the contents of a search tree to file (or stdout)
// warning: can produce arbitrarily large ouptut
// usees a pre-order tree walk
void write_to_file(SearchNode *node, ostream & output) {

    // Format is:
    // some hashtags ########
    // SearchNode title line (with "created by" move)
    // board with conf
    // score
    // children title
    // list of children and names

    // some hashtags
    output << "######################\n";

    // SearchNode title line
    output << "SearchNode at " << node
                << " (created by " << mtos(node->gs->board, node->move) << ")\n";

    // board with conf
    wr_board_conf(node->gs->board, output);

    output << "\nScore: " << node->score << "\n\n";

    // children title
    if (node->num_children == 0) {
        output << "Has no children.\n";
    } else {
        output << "Children:\n";

        // list of children and names
        for (int i = 0; i < node->num_children; ++i) {
            output << "Child " << i << ": " << node->children[i]
                    << " (" << mtos(node->children[i]->gs->board, node->children[i]->move)
                    << ") (" << node->children[i]->score << ")\n";
        }

    }

    output << "\n";


    // recurse for each child
    for (int i = 0; i < node->num_children; ++i) {
        write_to_file(node->children[i], output);
    }

    return;

}
