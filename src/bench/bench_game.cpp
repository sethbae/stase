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
#include <iomanip>
using std::setw;
#include <fstream>
using std::ifstream;
using std::ios;

#include <ctime>
using std::time;

#include "game.h"
#include "puzzle.h"
#include "../game/heur/heur.h"

void gamestate_recalculation() {
    Board b = fen_to_board("r1br2k1/1p3ppp/p1nqpb2/3nN3/3P1P2/1B2B3/PPN3PP/R2Q1RK1 w Qq - 0 1");
    // Board b = starting_pos();

    int k = 1000000;

    // Board * boards = new Board[k];
    Gamestate * states = new Gamestate[k];
    
    for (int i = 0; i < k; ++i) {
        states[i].board = b;
    }
    
    auto start = high_resolution_clock::now();
    
    int sum = 0;

    for (int j = 0; j < k; ++j) {
        states[j].recalculate_all();
        sum += states[j].fattack;
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

    delete [] states;
}


void calculating_check_with_gamestate() {
    Board b = fen_to_board("r1br2k1/1p3ppp/p1nqpb2/3nN3/3P1P2/1B2B3/PPN3PP/R2Q1RK1 w Qq - 0 1");
    // Board b = starting_pos();

    int k = 1000000;

    // Board * boards = new Board[k];
    Gamestate * states = new Gamestate[k];
    
    for (int i = 0; i < k; ++i) {
        states[i].board = b;
    }
    
    auto start = high_resolution_clock::now();
    
    int sum = 0;

    for (int j = 0; j < k; ++j) {
        // sum += in_check(states[j]);
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

    delete [] states;
}

void heuristic_evaluation() {
    
    vector<string> vec;
    read_all_fens(vec);
    
    // initialise all boards
    Gamestate * states = new Gamestate[vec.size()];
    for (int i = 0; i < vec.size(); ++i) {
        states[i].board = fen_to_board(vec[i]);
    }
    
    // start timer
    auto start = high_resolution_clock::now();
    
    double scores = 0.0;
    
    for (int i = 0; i < vec.size(); ++i) {
        scores += heur(states[i]);
    }
    
    /* end benchmark and return */
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    
    double millis = duration.count() / 1000.0;
    double per_board = duration.count() / (double) vec.size();
    
    cout << "Time taken for " << vec.size() << " boards was " << millis << " milliseconds ";
    cout << "(" << per_board << " microseconds per board)\n";  
    cout << "\t(Sum: " << scores << ")\n";
    cout << "\n";
    
    delete [] states;
    
}

double bench_metric(Metric *m, const string & name) {

    vector<string> vec;
    read_all_fens(vec);

    // initialise all boards
    Gamestate *states = new Gamestate[vec.size()];
    for (int i = 0; i < vec.size(); ++i) {
        states[i].board = fen_to_board(vec[i]);
    }

    // start timer
    auto start = high_resolution_clock::now();

    double scores = 0.0;

    for (int i = 0; i < vec.size(); ++i) {
        scores += (*m)(states[i].board);
    }

    /* end benchmark and return */
    auto stop = high_resolution_clock::now();
    double per_board = (double)(duration_cast<microseconds>(stop - start)).count() / (double)vec.size();

    cout << std::left << setw(20) << name << ": "
            << std::right << setw(8) << per_board
            << " microseconds     (Sum: " << setw(2) << scores << ")\n";

    delete [] states;

    return per_board;

}

int bench_game() {

    // heuristic_evaluation();

    double cum_micros = 0.0;

    cout << "Benchmarking individual metrics\n";

    for (int i = 0; i < METRICS_IN_USE; ++i) {
        cum_micros += bench_metric(METRICS[i], METRIC_NAMES[i]);
    }

    cout << std::left << setw(20) << "\nTotal time taken" << ": "
            << setw(6) << cum_micros << "microseconds\n";

    return 0;

}

