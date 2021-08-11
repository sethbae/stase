#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <fstream>
using std::ofstream;

#include "include/stase/board.h"
#include "include/stase/game.h"
#include "include/stase/search.h"

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

void analyse_position(const std::string & fen_str) {
    Gamestate gs;
    gs.board = fen_to_board(fen_str);

    pr_board(gs.board);

    vector<SearchNode*> nodes;

    nodes = depth_limited_search(gs, 5);
    //write_to_file("test", nodes);

    readable_printout(nodes, cout);
}

int main(int argc, char** argv) {

//    std::string fen_str;
//    if (argc <= 1) {
//        cout << "No board to analyse - using previous example instead\n";
//        fen_str = "5r2/3b2kp/6p1/8/5B2/2P5/2r2PBP/R5K1 w - - 1 35";
//    } else {
//        fen_str = std::string(argv[1]);
//    }
//
//    Board b = fen_to_board(fen_str);
//
//    heur_with_description(b);
//
//    analyse_position(fen_str);

    return 0;

}
