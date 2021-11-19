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

//bool test_mutate() {
//
//    Board b;
//    Move m;
//    int succ = 0, fail = 0;
//
//    // queen
//
//    // Qd3-h7
//    b = fen_to_board("8/3q4/8/8/8/3Q4/8/8 w - - 0 1");
//    m = {mksq(3, 2), mksq(7, 6), 0};
//    b.mutate(m);
//    if (board_to_fen(b) == "8/3q3Q/8/8/8/8/8/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // Qd3xd7
//    b = fen_to_board("8/3q4/8/8/8/3Q4/8/8 w - - 0 1");
//    m = {mksq(3, 2), mksq(3, 6), 0};
//    m.set_cap();
//    b.mutate(m);
//    if (board_to_fen(b) == "8/3Q4/8/8/8/8/8/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // rook
//
//    // Ra7-a2
//    b = fen_to_board("8/r2Q4/8/8/8/8/8/8 w - - 0 1");
//    m = {mksq(0, 6), mksq(0, 1), 0};
//    b.mutate(m);
//    if (board_to_fen(b) == "8/3Q4/8/8/8/8/r7/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // Ra7xd7
//    b = fen_to_board("8/r2Q4/8/8/8/8/8/8 b - - 0 1");
//    m = {mksq(0, 6), mksq(3, 6), 0};
//    m.set_cap();
//    b.mutate(m);
//    if (board_to_fen(b) == "8/3r4/8/8/8/8/8/8 b - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // bishop
//
//    // Bg4-e2
//    b = fen_to_board("8/3r4/8/8/6B1/8/8/8 w - - 0 1");
//    m = {mksq(6, 3), mksq(4, 1), 0};
//    b.mutate(m);
//    if (board_to_fen(b) == "8/3r4/8/8/8/8/4B3/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // Bg4xd7
//    b = fen_to_board("8/3r4/8/8/6B1/8/8/8 w - - 0 1");
//    m = {mksq(6, 3), mksq(3, 6), 0};
//    m.set_cap();
//    b.mutate(m);
//    if (board_to_fen(b) == "8/3B4/8/8/8/8/8/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // knight
//
//    // Nf8-g6
//    b = fen_to_board("5n2/3B4/8/8/8/8/8/8 b - - 0 1");
//    m = {mksq(5, 7), mksq(6, 5), 0};
//    b.mutate(m);
//    if (board_to_fen(b) == "8/3B4/6n1/8/8/8/8/8 b - - 0 1")
//       ++succ;
//    else
//       ++fail;
//
//    // Nf8xd7
//    b = fen_to_board("5n2/3B4/8/8/8/8/8/8 b - - 0 1");
//    m = {mksq(5, 7), mksq(3, 6), 0};
//    m.set_cap();
//    b.mutate(m);
//    if (board_to_fen(b) == "8/3n4/8/8/8/8/8/8 b - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // king (regular)
//
//    // Ke1xe2
//    b = fen_to_board("8/8/8/q7/8/8/4bn2/4K3 w - - 0 1");
//    m = {mksq(4, 0), mksq(4, 1), 0};
//    m.set_cap();
//    b.mutate(m);
//    if (board_to_fen(b) == "8/8/8/q7/8/8/4Kn2/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // Ke1xf2
//    b = fen_to_board("8/8/8/q7/8/8/4bn2/4K3 w - - 0 1");
//    m = {mksq(4, 0), mksq(5, 1), 0};
//    m.set_cap();
//    b.mutate(m);
//    if (board_to_fen(b) == "8/8/8/q7/8/8/4bK2/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // king (castle)
//
//    // ws
//    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
//    m = {mksq(4, 0), mksq(6, 0), 0};
//    m.set_cas();
//    m.set_cas_short();
//    b.mutate(m);
//    if (board_to_fen(b) == "r3k2r/8/8/8/8/8/8/R4RK1 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // wl
//    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
//    m = {mksq(4, 0), mksq(2, 0), 0};
//    m.set_cas();
//    m.unset_cas_short();
//    b.mutate(m);
//    if (board_to_fen(b) ==  "r3k2r/8/8/8/8/8/8/2KR3R w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // bs
//    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R b - - 0 1");
//    m = {mksq(4, 7), mksq(6, 7), 0};
//    m.set_cas();
//    m.set_cas_short();
//    b.mutate(m);
//    if (board_to_fen(b) ==  "r4rk1/8/8/8/8/8/8/R3K2R b - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // bl
//    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R b - - 0 1");
//    m = {mksq(4, 7), mksq(2, 7), 0};
//    m.set_cas();
//    m.unset_cas_short();
//    b.mutate(m);
//    if (board_to_fen(b) ==  "2kr3r/8/8/8/8/8/8/R3K2R b - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // pawn (regular)
//
//    // e5-e4
//    b = fen_to_board("r3k2r/8/8/4p3/8/8/8/R3K2R b - - 0 1");
//    m = {mksq(4, 4), mksq(4, 3), 0};
//    b.mutate(m);
//    if (board_to_fen(b) ==  "r3k2r/8/8/8/4p3/8/8/R3K2R b - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // pawn (e-p)
//
//    // f5xe6
//    b = fen_to_board("r3k2r/8/8/4pP2/8/8/8/R3K2R w - - 0 1");
//    m = {mksq(5, 4), mksq(4, 5), 0};
//    m.set_cap();
//    m.set_ep();
//    b.mutate(m);
//    if (board_to_fen(b) == "r3k2r/8/4P3/8/8/8/8/R3K2R w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // pawn (promotion)
//
//    // d8=Q
//    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
//    m = {mksq(3, 6), mksq(3, 7), 0};
//    m.set_prom_piece(QUEEN);
//    m.set_prom();
//    b.mutate(m);
//    if (board_to_fen(b) == "3Q4/8/8/8/8/8/8/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // d8=R
//    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
//    m = {mksq(3, 6), mksq(3, 7), 0};
//    m.set_prom_piece(ROOK);
//    m.set_prom();
//    b.mutate(m);
//    if (board_to_fen(b) == "3R4/8/8/8/8/8/8/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // d8=N
//    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
//    m = {mksq(3, 6), mksq(3, 7), 0};
//    m.set_prom_piece(KNIGHT);
//    m.set_prom();
//    b.mutate(m);
//    if (board_to_fen(b) == "3N4/8/8/8/8/8/8/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    // d8=B
//    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
//    m = {mksq(3, 6), mksq(3, 7), 0};
//    m.set_prom_piece(BISHOP);
//    m.set_prom();
//    b.mutate(m);
//    if (board_to_fen(b) == "3B4/8/8/8/8/8/8/8 w - - 0 1")
//        ++succ;
//    else
//        ++fail;
//
//    print_results(succ, fail, "MUTATE");
//    return fail == 0;
//}

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
    passed = test_mutate_hard() && passed;

    vector<bool (*)(void)> tests = {
    };

    for (auto test : tests) {
        if (!(*test)())
            exit(1);
    }

    return passed;
}
