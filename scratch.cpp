#include <iostream>
using std::cout;
#include <vector>
using std::vector;

#include "board/board.h"
#include "game/game.h"
#include "search/search.h"

int main(void) {

    Gamestate gs;
    //gs.board = starting_pos();
    gs.board = fen_to_board("4k3/4q3/8/8/1B6/8/8/4K3 w - - 0 1");
    
    vector<SearchNode*> nodes;
    
    nodes = depth_limited_search(gs, 5);
    
    readable_printout(nodes);
    
    /*for (SearchNode* node : nodes) {
        
        pr_board_conf(node->gs->board);   
        
    }
    
    Board b = fen_to_board("8/4Bk2/8/8/8/8/8/4K3 w - - 0 1");
    vector<Move> moves;
    
    legal_moves(b, moves);
    
    for (Move m : moves) {
        cout << movetosan(b, m) << " ";
    }
    cout << "\n";
    */
    return 0;

}
