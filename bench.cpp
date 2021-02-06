#include <iostream>
using std::cout;
#include <cstdlib>
using std::rand;
#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;

#include "board.cpp"

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
    pr(b, "\t");

    cout << "\n";

}

int main(void) {

    write_test();
    read_test();
    size_test();
    starting_pos_test();

}