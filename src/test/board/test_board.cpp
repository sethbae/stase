#include "board.h"
#include "../test.h"

#include <iostream>
using std::cout;
using std::cin;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <cstdlib>
using std::rand;


/* correctness tests for all code in the board directory */

void print_results(int succ, int fail, string test_name) {
    cout << test_name;
    for (int i = test_name.size(); i < 30; ++i)
        cout << " ";
        
    cout << "Passed " << succ << "/" << succ + fail << " tests\n";
    
    if (fail == 1) {
        cout << "*****ONE TEST FAILED*****\n";
    } else if (fail > 1) {
        cout << "*****SOME TESTS FAILED*****\n";
    }
}

/****************************************************************
 * Methods below here are not currently used but should be
 * refactored or deleted (BRD-10)
 ***************************************************************/

void starting_pos_test() {
    Board b = starting_pos();

    int sum = 0;
    for (int i = 0; i < 64; ++i) {
        sum += b.get(i);
    }

    cout << "Starting position test:\n";
    cout << "\tSum: " << sum << "\n";
    cout << "\tArrangement: \n\n";
    pr_board_conf(b, "\t");

    cout << "\n";

}

void board_from_fen_test() {

    cout << "Board from FEN test:\n\n";

    vector<string> test_fens = {
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "rnbqkb1r/pp1p1ppp/5n2/2p5/4p3/1N4P1/PPPPPPBP/RNBQK2R b KQkq - 1 5",
            "r4rk1/ppB2pp1/4p1p1/2P3q1/4Pn2/P1N2n2/2B2PPP/2R2RK1 w - - 0 24",
            "7k/1p6/p1p3p1/7p/1P2Q2P/P5P1/5r1K/5q2 w - h6 4 47",
            "2r2rk1/1p2bppp/p2pbn2/q1N1p3/2P1P3/N3BP2/PP2B1PP/2RR2K1 w - - 0 17"
    };

    for (int i = 0; i < 5; ++i) {
        cout << i << ".      Required:                    " << test_fens[i] << "\n";
        Board b = fen_to_board(test_fens[i]);
        //cout << "WS: " << b.get_cas_ws() << "\n";
        pr_board_conf(b, "\t");
        cout << "\n";
    }

}

void fen_from_board_test() {

    cout << "FEN from Board test:\n";

    vector<string> test_fens = {
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "rnbqkb1r/pp1p1ppp/5n2/2p5/4p3/1N4P1/PPPPPPBP/RNBQK2R b KQkq - 1 5",
            "r4rk1/ppB2pp1/4p1p1/2P3q1/4Pn2/P1N2n2/2B2PPP/2R2RK1 w - - 0 24",
            "7k/1p6/p1p3p1/7p/1P2Q2P/P5P1/5r1K/5q2 w - h6 4 47",
            "2r2rk1/1p2bppp/p2pbn2/q1N1p3/2P1P3/N3BP2/PP2B1PP/2RR2K1 w - - 0 17",
            "rnbqkbnr/pppp1ppp/8/4p3/8/4P3/PPPPKPPP/RNBQ1BNR b kq - 1 2"
    };

    for (int i = 0; i < (int) test_fens.size(); ++i) {
        Board b = fen_to_board(test_fens[i]);

        if (board_to_fen(b) == test_fens[i]) {
            cout << "\t" << i + 1 << ". Passed.\n";
        } else {
            cout << "\t" << i + 1 << ". Failed.\n";
        }

        // cout << "\tFEN: " << test_fens[i] << "\n";
        pr_board_conf(b, "\t");
        cout << "\n";

    }

}

bool test_board(void) {

    bool passed = true;

    passed = test_pieces() && passed;
    passed = test_set_get_square() && passed;
    passed = test_read_write_fens() && passed;
    passed = test_in_check() && passed;
    passed = test_castling() && passed;
    passed = test_mutate() && passed;
    passed = test_mutate_hard() && passed;

    vector<bool (*)(void)> tests = {
    };

    for (auto test : tests) {
        if (!(*test)())
            exit(1);
    }

    return passed;
}
