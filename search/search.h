#ifndef SEARCH_H
#define SEARCH_H

#include "../board/board.h"
#include "../game/game.h"


struct SearchNode {
    
    const Gamestate *gs;
    Eval score;
    Move move;
    unsigned short num_children;
    struct SearchNode **children;
    
};


std::vector<Move> depth_limited_search(const Gamestate &, int);


int subtree_size(SearchNode *);

#endif
