#include <iostream>
using std::cout;

#include <cstdlib>
using std::rand;

#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include "board.h"

/* This is a short benchmark program to test the read and write speeds of the board representation.
   It generates a random board position and then writes it to each of [k] boards.
   It then prints out the total time (millis) and the time per board (micros) */

void write_test() {
    
    int k = 10000000;

    auto boards = new Board[k];
    int ran[64];
    
    // initialise ran with random data
    for (int i = 0; i < 64; ++i) {
        ran[i] = rand();
    }

    /* start benchmark and write to boards */
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < 64; ++j) {
            boards[i].set(j, (Piece) ran[j]);
        }
    }
    
    auto stop = high_resolution_clock::now();
    
    
    int x = 0;
    for (int j = 0; j < k; ++j) {
        Board b = boards[j];
        for (int i = 0; i < 64; ++i) {
            x += b.get(i);
        }
    }
    
    /* end benchmark and return */    
    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Write test:\n";
    cout << "\tTime taken for " << k << " boards was " << millis << " ms ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\tSum: " << x << "\n";
    cout << "\n";

    delete [] boards;
}

void read_test() {
    
    int k = 10000000;

    auto boards = new Board[k];
    int ran[64];
    
    // initialise ran with random data
    for (int i = 0; i < 64; ++i) {
        ran[i] = rand();
    }
    
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < 64; ++j) {
            boards[i].set(j, (Piece) ran[j]);
        }
    }
    
    /* start benchmark and read from boards */

    auto start = high_resolution_clock::now();

    int x = 0;
    for (int j = 0; j < k; ++j) {
        Board b = boards[j];
        for (int i = 0; i < 64; ++i) {
            x += b.get(i);
        }
    }

    /* end benchmark and return */
    auto stop = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Read test:\n";
    cout << "\tTime taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\tSum: " << x << "\n";
    cout << "\n";
    
    delete [] boards;
}

void legal_move_test() {
        
    int k = 100000;

    auto boards = new Board[k];
    
    for (int i = 0; i < k; ++i) {
        //boards[i] = starting_pos();
        //boards[i] = fen_to_board("8/8/8/3Q4/8/8/8/8 w - - 0 1");
        boards[i] = fen_to_board("r1b1k2r/p1pq2p1/1pn1p1p1/3pP2p/3P4/B1P2Q2/P1P2RPP/R5K1 b kq - 1 15");
    }
    
    /* start benchmark and read from boards */

    vector<string> moves;

    auto start = high_resolution_clock::now();

    int x = 0;
    for (int j = 0; j < k; ++j) {
        Board b = boards[j];
        get_all_legal_moves(b, moves);
        x += moves.size();
        moves.clear();
    }

    /* end benchmark and return */
    auto stop = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Legal move test:\n";
    cout << "\tTime taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\tMoves found: " << (x / k) << "\n";
    cout << "\n";
    
    delete [] boards;
}

void legal_move_test2() {
        
    int k = 100000;

    auto boards = new Board[k];
    
    for (int i = 0; i < k; ++i) {
        //boards[i] = starting_pos();
        //boards[i] = fen_to_board("8/8/8/3Q4/8/8/8/8 w - - 0 1");
        boards[i] = fen_to_board("r1b1k2r/p1pq2p1/1pn1p1p1/3pP2p/3P4/B1P2Q2/P1P2RPP/R5K1 b kq - 1 15");
    }
    
    /* start benchmark and read from boards */

    vector<string> moves;

    auto start = high_resolution_clock::now();

    int x = 0;
    for (int j = 0; j < k; ++j) {
        
        Board b = boards[j];
        Bitmap bmap = (Bitmap) 0;
        
        for (int i = 0; i < 8; ++ i) {
            for (int j = 0; j < 8; ++j) {
                Piece p = b.get(mksq(i, j));
                if (colour(p) == b.colour_to_move()) {
                    piecemoves(b, mksq(i, j), bmap);
                    x += (unsigned) bmap;
                }
            }
        }
        
    }

    /* end benchmark and return */
    auto stop = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Legal move test:\n";
    cout << "\tTime taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\tMoves found: " << (x / k) << "\n";
    cout << "\n";
    
    delete [] boards;
}

void size_test() {

    cout << "Size test:\n";
    cout << "\tSize of Square: " << sizeof(Square) << "\n";
    cout << "\tSize of Board: " << sizeof(Board) << "\n";
    cout << "\n";

}

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

void write_config_test() {
    int k = 10000000;

    auto boards = new Board[k];
    
    /* start benchmark and write configs into boards */
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < k; ++i) {
        boards[i].set_white(1);

        boards[i].set_cas_ws(1);
        boards[i].set_cas_wl(1);
        boards[i].set_cas_bs(1);
        boards[i].set_cas_bl(1);

        boards[i].set_ep_exists(0);
        boards[i].set_halfmoves(0);
        boards[i].set_wholemoves(1);
    }
    
    /* end benchmark and return */
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Write config test:\n";
    cout << "\tTime taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";
    cout << "\n";
    
    delete [] boards;
}

/*void write_parsed_config_test() {
    int k = 10000000;

    auto boards = new Board[k];
    
    // start benchmark and write configs into boards
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < k; ++i) {
        stringstream conf("w KQkq e6 0 3");
        fill_config(boards[i], conf);
    }
    
    // end benchmark and return 
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Write parsed config test:\n";
    cout << "\tTime taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";
    cout << "\n";
    
    delete [] boards;
}*/

void read_config_test() {
    int k = 10000000;

    auto boards = new Board[k];
    
    
    for (int i = 0; i < k; ++i) {
        boards[i].set_white(1);

        boards[i].set_cas_ws(1);
        boards[i].set_cas_wl(1);
        boards[i].set_cas_bs(1);
        boards[i].set_cas_bl(1);

        boards[i].set_ep_exists(0);
        boards[i].set_halfmoves(42);
        boards[i].set_wholemoves(17);
    }

    /* start benchmark and read configs from boards */
    auto start = high_resolution_clock::now();
    
    int x = 0;
    for (int i = 0; i < k; ++i) {
        x += boards[i].get_white();
        x += boards[i].get_cas_ws();
        x += boards[i].get_cas_wl();
        x += boards[i].get_cas_bs();
        x += boards[i].get_cas_bl();

        x += boards[i].get_ep_exists();
        x += boards[i].get_halfmoves();
        x += boards[i].get_wholemoves();
    }

    /* end benchmark and return */
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Read config test:\n";
    cout << "\tTime taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";
    cout << "\tSum: " << x << "\n";
    cout << "\n";
    
    delete [] boards;
}

int main(void) {

    // write_test();
    // read_test();
    // size_test();
    // starting_pos_test();
    // board_from_fen_test();
    // fen_from_board_test();
    // write_config_test();
    // read_config_test();
    // write_parsed_config_test();

    legal_move_test();
    legal_move_test2();
    
}
