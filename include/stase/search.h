#ifndef STASE_SEARCH_H
#define STASE_SEARCH_H

#include "board.h"
#include "game.h"

#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;

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

int subtree_size(SearchNode *);

void readable_printout(std::vector<SearchNode*> &, std::ostream &);

void write_to_file(SearchNode*, std::ostream &);

#endif //STASE_SEARCH_H
