#ifndef STASE_SEARCH_H
#define STASE_SEARCH_H

#include "board.h"
#include "game.h"

#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
#include <pthread.h>

struct SearchNode {

    Gamestate *gs;
    Eval score;
    Move move;
    unsigned short num_children;
    SearchNode ** children;
    SearchNode * best_child;

};

std::vector<Move> iterative_deepening_search(const std::string &, int);
void search_indefinite(SearchNode *);
void delete_tree(SearchNode *);

int subtree_size(SearchNode *);
void write_to_file(SearchNode*, std::ostream &);


void run_in_background(const std::string & fen);
void stop_engine();
Move fetch_best_move();
int fetch_node_count();

#endif //STASE_SEARCH_H
