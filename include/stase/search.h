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

struct DepthLimiter {

    int cutoff;

    DepthLimiter(int n) : cutoff(n) {}

    inline bool check(int depth) {
        return depth < cutoff;
    }

};

struct TimeLimiter {

    int cutoff;

    TimeLimiter(int n) : cutoff(n) {}

    inline bool check() {
        // TODO get the current time and check if we've passed it
        return true;
    }

};

std::vector<Move> iterative_deepening_search(const std::string &, int);
void search_indefinite(SearchNode *);
void delete_tree(SearchNode *);

int subtree_size(SearchNode *);
void write_to_file(SearchNode*, std::ostream &);


pthread_t run_in_background(const std::string & fen);
Move cancel_and_fetch_move(pthread_t);
void free_memory_for_exit();

#endif //STASE_SEARCH_H
