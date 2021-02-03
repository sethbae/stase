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

/* This is a short benchmark program to test the write speeds of the board representation.
   It generates a random board position and then writes it to each of [k] boards.
   It then prints out the total time (millis) and the time per board (micros) */

int main(void) {

    int k = 1000000;

    auto boards = new Board[k];
    int ran[64];
    
    // initialise ran with random data
    for (int i = 0; i < 64; ++i) {
        ran[i] = random();
    }

    /* start benchmark and write to boards */
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < 64; ++j) {
            boards[i].set(j, ran[j]);
        }
    }
    
    /* end benchmark and return */
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    
    cout << "Time taken for " << k << " boards was " << millis << " milliseconds\n";
    
    double per_board = duration.count() / (double) k;
    
    cout << "Which is " << per_board << " microseconds per board\n";
    
}
