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

#include <fstream>
using std::ifstream;
using std::ios;

#include <ctime>
using std::time;


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

/*void legal_move_test() {
        
    int k = 100000;

    auto boards = new Board[k];

    Board b = fen_to_board("r1b1k2r/p1pq2p1/1pn1p1p1/3pP2p/3P4/B1P2Q2/P1P2RPP/R5K1 b kq - 1 15");
    
    for (int i = 0; i < k; ++i) {
        //boards[i] = starting_pos();
        //boards[i] = fen_to_board("8/8/8/3Q4/8/8/8/8 w - - 0 1");
        boards[i] = b;
    }
    
    // start benchmark and read from boards

    auto start = high_resolution_clock::now();

    Bitmap vacancy = vacancy_map(b);
    Bitmap enemy_pieces = custom_map(b, [] (Square, Piece p, Ptype t) { return colour(p) != t; });

    int sum = 0;
    for (int j = 0; j < k; ++j) {
        // Board b = boards[j];
        // get_all_legal_moves(b, moves);
        // x += moves.size();
        // moves.clear();

        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                sum += get_obstructed_move_map(b, mksq(x, y), vacancy, enemy_pieces);
            }
        }
    }

    // end benchmark and return
    auto stop = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Legal move test:\n";
    cout << "\tTime taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\tMoves found: " << (sum / k) << "\n";
    cout << "\n";
    
    delete [] boards;
}*/

void legal_move_test2() {
        
    int k = 1000000;

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
                    x += (unsigned) piecemoves(b, mksq(i, j));
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

/* generates a random board with some number of the desired piece and some number of pawns,
    all dotted around on any square at all. Warning, they may overwrite each other, so the
    actual numbers may vary. Pawns won't overwrite the desired piece, however.
   If you ask for pawns, it will randomly dot a different piece around. */
Board random_board(const Piece p, int num_of_piece, int num_of_other_pieces) {

    Board b = empty_board();
    
    for (int i = 0; i < num_of_other_pieces; ++i) {
        Square sq = mksq(rand() % 8, rand() % 8);
        
        if (type(p) != PAWN) {
            b.set(sq, (rand() % 2) ? W_PAWN : B_PAWN);
        } else {
            b.set(sq, (rand() % 2) ? W_QUEEN : B_QUEEN);
        }
    }

    for (int i = 0; i < num_of_piece; ++i) {
        Square sq = mksq(rand() % 8, rand() % 8);
        b.set(sq, p);
    }

    return b;

}

void piecemoves_bench(const Piece target_piece, int density) {

    int k = 10000000;

    Board *boards = new Board[k];
    
    for (int i = 0; i < k; ++i) {
        boards[i] = random_board(target_piece, density, density);
    }
    
    auto start = high_resolution_clock::now();

    int sum = 0;
    for (int j = 0; j < k; ++j) {
        
        Board b = boards[j];
        Bitmap bmap = (Bitmap) 0;
        
        for (Square s = mksq(0, 0); val_y(s); inc_y(s)) {
            for ( ; val_x(s); inc_x(s)) {
                Piece p = b.get(s);
                if (p == target_piece)
                    sum += (unsigned) piecemoves_ignore_check(b, s);
            }
            reset_x(s);
        }
        
    }

    /* end benchmark and return */
    auto stop = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Time taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\t(Sum: " << sum << ")\n";
    cout << "\n";
    
    delete [] boards;

}

/******************************************************************
 * This will read in puzzles from the lichess database, in FEN,
 * provided that you download and unzip it to the same directory.
 * It reads puzzles sequentially from a randomly selected offset,
 * which uses the current time as a seed, so will differ between
 * executions.
 * 
 * Be warned this may make it hard to reproduce bugs/errors if you
 * do not see or store the FEN somewhere.
 *
 * The puzzle database is available here: 
 *       https://database.lichess.org/#puzzles
 ******************************************************************/   

/* fill the given vector with [num] randomly selected puzzles 
    these puzzles will always differ since they are selected using a random seed */
bool read_fens(unsigned num, vector<string> & vec) {
       
    ifstream file;
    
    file.open("../puzzles/lichess_db_puzzle.csv", ios::in);
    if (!file) {
        cout << "WARNING: could not read puzzle csv\n";
        return false;
    }
    
    string s;
    
    // initialise the seed with current time
    srand(time(nullptr));
    
    // and skip a random number of puzzles (different every time)
    unsigned x = rand() % 10000;
    while (x--)
        getline(file, s);
    
    // before reading the required number of puzzles
    for (; getline(file, s) && num > 0; --num) {
        
        // FEN is second field of CSV
        s = s.substr(s.find_first_of(',') + 1);
        s = s.substr(0, s.find_first_of(','));
        vec.push_back(s);
        
    } 

    return true;

}

void legal_moves_puzzles() {

    int k = 500000;

    vector<string> fens;
    
    read_fens(k, fens);
    
    Board *boards = new Board[k];
    
    int i = 0;
    for (string s : fens)
        boards[i++] = fen_to_board(s);
    
    auto start = high_resolution_clock::now();
    
    int sum = 0;
    for (int j = 0; j < k; ++j) {
        
        Board b = boards[j];
        Bitmap bmap = (Bitmap) 0;
        
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                Piece p = b.get(mksq(x, y));
                if (colour(p) == b.colour_to_move()) {
                    sum += (unsigned) piecemoves_ignore_check(b, mksq(x, y));
                }
            }
        }
        
    }
    
    /* end benchmark and return */
    auto stop = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Time taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\t(Sum: " << sum << ")\n";
    cout << "\n";
    
    delete [] boards;
    
}

void board_iteration_bench(bool use_mksq) {

    int k = 10000000;

    Board *boards = new Board[k];
    
    for (int i = 0; i < k; ++i) {
        boards[i] = random_board(W_PAWN, 5, 5);
    }
    
    auto start = high_resolution_clock::now();
    
    int sum = 0;
    
    if (use_mksq) {

        for (int j = 0; j < k; ++j) {
            
            Board b = boards[j];
            Bitmap bmap = (Bitmap) 0;
            
            for (int x = 0; x < 8; ++x) {
                for (int y = 0; y < 8; ++y) {
                    sum += (unsigned) b.get(mksq(x, y));
                }
            }
            
        }
    } else {

        for (int j = 0; j < k; ++j) {
            
            Board b = boards[j];
            Bitmap bmap = (Bitmap) 0;
            
            for (Square s = mksq(0, 0); val_y(s); inc_y(s)) {
                for ( ; val_x(s); inc_x(s)) {
                    sum += (unsigned) b.get(s);
                }
                reset_x(s);
            }
            
        }
    }
    /* end benchmark and return */
    auto stop = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Time taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\t(Sum: " << sum << ")\n";
    cout << "\n";
    
    delete [] boards;

}

/*
    Passing function by value
    Queen:  2.5862, 2.5595 micros       piecemoves_bench(piece, 5, 5);
    Rook:   1.6306, 1.6089 
    Bishop: 1.3708, 1.3902
    Knight: 0.9504, 0.9442
    King:   0.9694, 0.9851
    Pawn:   0.6719, 0.6754
    
    Puzzles: 2.1818, 2.1015 micros
    
    Passing function by pointer
    Queen:  2.4390, 2.4452
    Rook:   1.6444, 1.6386
    Bishop: 1.3828, 1.3942
    
    Passing function by reference
    Queen:  2.5701, 2.5866
    Rook:   1.6208, 1.6280
    Bishop: 1.3542, 1.3613
    
*/

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

    //legal_move_test();
    //legal_move_test2();
    
    //piecemoves_bench(W_QUEEN, 5);
    
    board_iteration_bench(true);
    
    //legal_moves_puzzles();
    
    //pr_board(random_board(W_QUEEN, 5, 5));
    
}
