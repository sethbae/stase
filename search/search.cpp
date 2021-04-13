#include "../game/game.h"
#include "search.h"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;
#include <functional>

// allocate a new SearchNode with the given gamestate, but modified by the move.
// Original gamestate is unmodified.
SearchNode *new_node(const Gamestate & gs, Move m) {

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

// computes the score of each node as either an Eval computed by heur, or as the maximum
// (resp. minimum) Eval of its successors. Returns a vector of pointers to the nodes which
// make up the best line. These are in reverse order: first successor is last in the list.
// The node given is also in the list (at the end).
vector<SearchNode*> minimax(SearchNode* node) {

    if (node->num_children == 0) {
        node->score = heur(*(node->gs));
        vector<SearchNode*> vec = { node };
        return vec;
    }
    
    // assign the correct comparison according to colour
    std::function<bool(Eval,Eval)> compare;
    
    if (node->gs->board.get_white())
        compare = [](Eval a, Eval b) -> bool{ return a > b; };
    else
        compare = [](Eval a, Eval b) -> bool{ return a < b; };
    
    // start with the score of the first child
    vector<SearchNode*> best_line = minimax(node->children[0]);
    Eval best_score = node->children[0]->score;
    
    // and then check the others for improvements
    for (int i = 1; i < node->num_children; ++i) {
    
        vector<SearchNode*> other_line = minimax(node->children[i]);
        Eval other_score = node->children[i]->score;
        
        if (compare(other_score, best_score)) {
            best_line = other_line;
            best_score = other_score;
        }
    
    }
    
    best_line.push_back(node);
    return best_line;

}

vector<SearchNode*> depth_limited_search(const Gamestate & start, int depth) {

    SearchNode *root = new SearchNode;
    *root = { &start, (Eval) 0, empty_move(), 0, nullptr };
    
    vector<SearchNode*> openlist = { root };
    
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
    cout << "Number of nodes in tree: " << subtree_size(root) << "\n";
    
    return minimax(root);

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

void readable_printout(vector<SearchNode*> & nodes) {
    
    auto itr = nodes.rbegin();
    
    pr_board((*itr)->gs->board);
    cout << "\nBest line: ";
    itr++;
    
    while (itr != nodes.rend()) {
    
        SearchNode *node = *itr;
        
        cout << movetosan(node->gs->board, node->move) << " ";
        //pr_board(node->gs->board);
        
        itr++;
    
    }
    
    cout << "\n";
    
}


