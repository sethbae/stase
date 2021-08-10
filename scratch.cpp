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

int main(int argc, char** argv) {

    /*
    Gamestate gs;
    //gs.board = starting_pos();
    //gs.board = fen_to_board("4k3/4q3/8/8/1B6/8/8/4K3 w - - 0 1");
    gs.board = fen_to_board("1rr4k/pb3pp1/1p1p1q2/4P3/3P1n2/P2B1N2/1P1Q1PPP/R4RK1 b - - 0 20");
    gs.board = fen_to_board("2k5/4qpp1/p3n3/3pP2r/2P3Q1/P7/3B3P/4R1K1 w - - 2 32");
    
    pr_board(gs.board);
    
    vector<SearchNode*> nodes;
    
    nodes = depth_limited_search(gs, 5);
    //write_to_file("test", nodes);
    
    readable_printout(nodes, cout);
    */
    
    
    // Board b = fen_to_board("5r2/3b2kp/6p1/8/5B2/2P5/2r2PBP/R5K1 w - - 1 35");
    // Board b = fen_to_board("rn3rk1/pp4pp/3bq3/3p1p2/3Pn3/1Q3N2/PP2BPPP/R1B1R1K1 w - - 2 15");
    if (argc <= 0) {
        cout << "No board to analyse\n";
        exit(0);
    }
    std::string str(argv[1]);
    Board b = fen_to_board(str);
/*
    pr_board(b);

    
    cout << "\n";
       
    
    cout << "Alpha\n";
    cout << piece_activity_alpha(b) << "\n\n";
    
    cout << "Beta\n";
    cout << piece_activity_beta(b) << "\n\n";
    
    cout << "Gamma\n";
    cout << piece_activity_gamma(b) << "\n\n";
    
    
    cout << "Centre control: " << centre_control(b) << "\n\n";
    
    cout << "Open line control: " << open_line_control(b) << "\n\n";

    display_control_counts(b);
    
    cout << "\nOverall evaluation: " << human_eval(heur(b)) << "\n";
    */
    
    heur_with_description(b);
    
    return 0;

}
