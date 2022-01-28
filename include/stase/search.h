#ifndef STASE_SEARCH_H
#define STASE_SEARCH_H

#include "board.h"
#include "game.h"

#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
#include <pthread.h>

struct SearchNode {

    Gamestate * gs;
    CandSet * cand_set;
    Eval score;
    Move move;
    unsigned short num_children;
    SearchNode ** children;
    SearchNode * best_child;
    SearchNode * best_trust_child;
    unsigned short visit_count;

};

std::vector<Move> iterative_deepening_search(const std::string &, int);
std::vector<Move> greedy_search(const std::string &, int);
std::vector<Move> greedy_search(SearchNode *, int);

void search_indefinite(SearchNode *);
void delete_tree(SearchNode *);

int subtree_size(SearchNode *);
int subtree_depth(SearchNode *);
void write_to_file(SearchNode*, std::ostream &);
void write_to_file_recursively(SearchNode*, std::ostream &);

void run_in_background(const std::string & fen);
void run_with_node_limit(const std::string & fen, int);

void stop_engine(bool cleanup=true);
Move fetch_best_move();
int fetch_node_count();
SearchNode * fetch_root();

#endif //STASE_SEARCH_H
