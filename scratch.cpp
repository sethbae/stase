#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <fstream>
using std::ofstream;

#include "include/stase/board.h"
#include "include/stase/game.h"
#include "include/stase/search.h"
#include "include/stase/puzzle.h"
#include "src/game/cands/cands.h"
#include "src/bench/bench.h"
#include "src/test/test.h"


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
//
//void record_tree_in_file(const std::string & filename, const SearchNode * nodes) {
//
//    ofstream file;
//    file.open(filename, std::ios::out);
//
//    file << "This file contains engine analysis of the following position\n";
//
//    readable_printout(nodes, file);
//
//    file << "NB score is inherited from best successors, computed only for leaves\n";
//    file << "\nHere are the nodes:\n";
//
//    write_to_file(nodes[nodes.size() - 1], file);
//    file.close();
//
//}

//void analyse_position(const std::string & fen_str) {
//    Gamestate gs;
//    gs.board = fen_to_board(fen_str);
//
//    pr_board(gs.board);
//
//    vector<SearchNode*> nodes;
//
//    nodes = depth_limited_search(gs, 5);
//    //write_to_file("test", nodes);
//
//    readable_printout(nodes, cout);
//}

int main(int argc, char** argv) {

    std::string fen = "8/1k5p/8/8/8/8/7P/1K6 w - - 0 1";
    pr_board(fen_to_board(fen));

    iterative_deepening_search(fen, 10);

    return 0;

}
