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
#include "puzzle.h"
#include "../bench.h"


struct BoardPairParam {
    Board & board;
    Board & blank_board;
};

int write_board_xy(const BoardPairParam & param) {
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Square temp;
            param.blank_board.set(temp = mksq(x, y), param.board.get(temp));
        }
    }
    return param.board.get_white() ? 1 : 0;
}

int write_board_yx(const BoardPairParam & param) {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Square temp;
            param.blank_board.set(temp = mksq(x, y), param.board.get(temp));
        }
    }
    return param.board.get_white() ? 1 : 0;
}

int write_board_64(const BoardPairParam & param) {
    for (int x = 0; x < 64; ++x) {
        param.blank_board.set(x, param.board.get(x));
    }
    return param.board.get_white() ? 1 : 0;
}

/**
 * Benchmarks the read/write time for a whole board. It sets every square of a blank board
 * to a value got from a puzzle board. There are three different strategies compared for
 * iterating over the board: see above for the implementations.
 */
void bench_board_write() {

    std::vector<Board> boards;
    puzzle_boards(boards);

    std::vector<BoardPairParam> params;
    for (int i = 0; i < boards.size(); ++i) {
        Board b = empty_board();
        params.push_back(BoardPairParam{boards[i], b});
    }

    bench("board-get-set-xy", MICROS, params.data(), params.size(), &write_board_xy);
    bench("board-get-set-yx", MICROS, params.data(), params.size(), &write_board_yx);
    bench("board-get-set-64", MICROS, params.data(), params.size(), &write_board_64);

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
//bool read_fens(unsigned num, vector<string> & vec) {
//
//    ifstream file;
//
//    file.open("../puzzles/lichess_db_puzzle.csv", ios::in);
//    if (!file) {
//        cout << "WARNING: could not read puzzle csv\n";
//        return false;
//    }
//
//    string s;
//
//    // initialise the seed with current time
//    srand(time(nullptr));
//
//    // and skip a random number of puzzles (different every time)
//    unsigned x = rand() % 10000;
//    while (x--)
//        getline(file, s);
//
//    // before reading the required number of puzzles
//    for (; getline(file, s) && num > 0; --num) {
//
//        // FEN is second field of CSV
//        s = s.substr(s.find_first_of(',') + 1);
//        s = s.substr(0, s.find_first_of(','));
//        vec.push_back(s);
//
//    }
//
//    return true;
//
//}

void legal_moves_puzzles() {

    int k = 2000000;

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
        sum += legal_moves(b).size();
        
//        for (int x = 0; x < 8; ++x) {
//            for (int y = 0; y < 8; ++y) {
//                Piece p = b.get(mksq(x, y));
//                if (colour(p) == b.colour_to_move()) {
//                    sum += (unsigned) piecemoves_ignore_check(b, mksq(x, y));
//                }
//            }
//        }
        
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

            
            for (int x = 0; x < 8; ++x) {
                for (int y = 0; y < 8; ++y) {
                    sum += (unsigned) b.get(mksq(x, y));
                }
            }
            
        }
    } else {

        for (int j = 0; j < k; ++j) {
            
            Board b = boards[j];

            
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

void attack_map_generation() {
    // Board b = fen_to_board("r1br2k1/1p3ppp/p1nqpb2/3nN3/3P1P2/1B2B3/PPN3PP/R2Q1RK1 w Qq - 0 1");
    Board b = starting_pos();

    int k = 100000;

    Board * boards = new Board[k];
    
    for (int i = 0; i < k; ++i) {
        boards[i] = b;
    }
    
    auto start = high_resolution_clock::now();
    
    int sum = 0;

    for (int j = 0; j < k; ++j) {
        sum += attack_map(boards[j], WHITE);
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

    
    // pr_bitmap(attack_map(b, WHITE));
    // pr_bitmap(1ULL);

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

void compare_check() {

    const int k = 1000000;

    vector<string> fens;
    read_fens(k, fens);
    
    Board* boards = new Board[fens.size()];
    
    for (int i = 0; i < (int) fens.size(); ++i)
        boards[i] = fen_to_board(fens[i]);
        
    int sum = 0;
    int count = 0;
           
    // first bench - using in check hard
           
    auto start = high_resolution_clock::now();
    
    /***********************************************/   
    while (count < k) {
        for (int i = 0; i < (int) (int) fens.size() && count < k; ++i) {
            sum += (int) in_check_hard(boards[i]);
            ++count;
        }
    }
    /***********************************************/
    
    /* end benchmark and return */
    auto stop = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(stop - start);
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) k;
    
    cout << "Time taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\t(Sum: " << sum << ")\n";
    cout << "\n";
    
    // second bench - using attack map
    
    sum = 0;
    count = 0;
    
    start = high_resolution_clock::now();
    
    /***********************************************/
    while (count < k) {
        for (int i = 0; i < (int) fens.size() && count < k; ++i) {
            sum += (int) in_check_attack_map(boards[i], BLACK);
            ++count;
        }
    }
    /***********************************************/
    
    /* end benchmark and return */
    stop = high_resolution_clock::now();
    
    duration = duration_cast<microseconds>(stop - start);
    millis = duration.count() / 1000.0;
    per_board = duration.count() / (double) k;
    
    cout << "Time taken for " << k << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\t(Sum: " << sum << ")\n";
    cout << "\n";

}

int bench_board(void) {

    // attack_map_generation();

    // compare_check();

    write_test();
    // read_test();
    // size_test();
    // starting_pos_test();
    // board_from_fen_test();
    // fen_from_board_test();
    // write_config_test();
    // read_config_test();
    // write_parsed_config_test();

    bench_board_write();

    legal_moves_puzzles();

    // legal_move_test();
    // legal_move_test2();
    
    // piecemoves_bench(W_QUEEN, 5);
    
    // board_iteration_bench(true);
    
    // legal_moves_puzzles();
    
    // pr_board(random_board(W_QUEEN, 5, 5));
    
    return 0;
}
