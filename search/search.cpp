#include "../game/game.h"
#include "search.h"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;


// allocate a new SearchNode with the given gamestate, but modified by the move.
// Original gamestate is unmodified.
SearchNode *new_node(const Gamestate & gs, Move m) {

    Gamestate *new_gs = new Gamestate;
    new_gs->board = gs.board.successor(m);
    
    SearchNode *new_node = new SearchNode;
    new_node->gs = new_gs;
    new_node->score = (Eval) 0;
    new_node->move = empty_move();
    new_node->num_children = 0;
    new_node->children = nullptr;
    
    return new_node;

}

vector<Move> depth_limited_search(const Gamestate & start, int depth) {

    SearchNode root = { &start, (Eval) 0, empty_move(), 0, nullptr };
    
    vector<SearchNode*> openlist = { &root };
    
    while (depth--) {
    
        vector<SearchNode*> next_openlist;
    
        // for every node in open list
        for (SearchNode *current : openlist) {
        
            // get legal moves
            vector<Move> lmoves = cands(*current->gs);
            //legal_moves(current->gs->board, lmoves);
                    
            if (lmoves.size() > 0) {
            
                // initialise children array
                current->children = new SearchNode*[lmoves.size()];
                current->num_children = lmoves.size();
                
                // populate children array
                for (int i = 0; i < lmoves.size(); ++i) {
                    current->children[i] = new_node(*(current->gs), lmoves[i]);
                    // adding each new child to the new open list
                    next_openlist.push_back(current->children[i]);
                    
                    /*Board b = current->children[i]->gs->board;
                    cout << "Move: " << movetosan(b, lmoves[i]) << "\n";
                    pr_board(b);
                    cout << "\n\n";*/
                    
                }
                
            }
            
            else {
                // no children
            }
            
        }
        
        openlist = next_openlist;
        
    }
    
    // tree walk to find the minimax line of best play
    //cout << "Root has " << root.num_children << " children\n";
    cout << "Number of nodes in tree: " << subtree_size(&root) << "\n";
    
    vector<Move> v;
    
    return v;

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





