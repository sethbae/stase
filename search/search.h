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


std::vector<SearchNode*> depth_limited_search(const Gamestate &, int);

int subtree_size(SearchNode *);

void readable_printout(std::vector<SearchNode*> &, std::ostream &);

void write_to_file(SearchNode*, std::ostream &);

#endif
