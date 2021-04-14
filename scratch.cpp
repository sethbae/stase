#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <fstream>
using std::ofstream;

#include "board/board.h"
#include "game/game.h"
#include "search/search.h"

void record_tree_in_file(const std::string & filename, vector<SearchNode*> & nodes) {

    ofstream file;
    file.open(filename, std::ios::out);
    
    file << "This file contains engine analysis of the following position\n";
    
    readable_printout(nodes, file);
    
    file << "NB score is inherited from best successors, computed only for leaves\n";
    file << "\nHere are the nodes:\n";
    
    write_to_file(nodes[nodes.size() - 1], file);
    file.close();

}

int main(void) {

    Gamestate gs;
    //gs.board = starting_pos();
    //gs.board = fen_to_board("4k3/4q3/8/8/1B6/8/8/4K3 w - - 0 1");
    gs.board = fen_to_board("3r1rk1/2pn1pp1/p1p2b1p/1p2pP2/4P3/1P1P2QP/PBq1NRP1/3R2K1 w - - 0 18");
    
    vector<SearchNode*> nodes;
    
    nodes = depth_limited_search(gs, 5);
    
    //write_to_file("test", nodes);
    
    readable_printout(nodes, cout);
    
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
