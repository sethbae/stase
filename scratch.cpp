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

    vector<std::string> test_fens = {"3k4/8/8/8/8/8/4p3/3K4 w K - 0 1",  // pawn checking
                                "3k4/2P5/8/8/8/8/8/3K4 b K - 0 1",
                                "3k4/3P4/8/8/8/8/3p4/3K4 w - - 0 1", // pawn not checking
                                "3k4/3P4/8/8/8/8/3p4/3K4 b - - 0 1",
                                "3k4/8/8/8/8/5b2/8/3K4 w K - 0 1",  // bishop checking
                                "7k/8/5B2/8/8/8/8/3K4 b K - 0 1",
                                "1k6/8/8/8/8/5b2/4b3/6K1 w - - 0 1", // bishop not checking
                                "1k6/3B4/2B5/8/8/8/8/6K1 b - - 0 1",
                                "1k6/8/4n3/8/3K4/8/8/8 w K - 0 1",  // knight checking
                                "1k6/8/2N5/8/3K4/8/8/8 b K - 0 1",
                                "1k6/8/3n4/8/3Kn3/8/3n4/8 w - - 0 1", // knight not checking
                                "1k6/2N5/1N6/3N4/3K4/8/8/8 b - - 0 1",
                                "1k1r4/8/8/8/3K4/8/8/8 w K - 0 1",  // rook checking
                                "1k1R4/8/8/8/3K4/8/8/8 b K - 0 1",
                                "1kr1r3/8/8/8/3K4/7r/8/8 w - - 0 1", // rook not checking
                                "1k6/2RR4/8/8/3K4/8/8/7R b - - 0 1",
                                "1k6/8/5q2/8/3K4/8/8/8 w K - 0 1",  // queen checking
                                "1k6/8/3Q4/8/3K4/8/8/8 b K - 0 1",
                                "1k4q1/8/2q5/5q2/3K4/1q6/8/8 w - - 0 1", // queen not checking
                                "1k6/3Q4/Q7/3Q4/3K4/8/2Q5/2Q5 b - - 0 1",
                                "1k6/8/8/8/2n5/q2r4/2B5/1KR5 w - - 0 1", // medley no check
                                "1k6/2rQN3/B1QB4/1P6/2n5/q2r4/RP6/1K6 b - - 0 1",
                                "1k6/2rQN3/B1QB4/1P6/2n5/q2r4/RP6/1K5q w K - 0 1", //medley check
                                "1k6/P1rQN3/B1QB4/1P6/2n5/q2r4/RP6/1K5q w K - 0 1"
    };

    for (const std::string & s : test_fens) {
        cout << board_to_fen(fen_to_board(s));
        if (board_to_fen(fen_to_board(s)) == s)
            cout << "Pass\n";
        else
            cout << "Fail\n";
    }

    cout << board_to_fen(starting_pos()) << "\n";

    return 0;

}
