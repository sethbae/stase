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
#include <cstring>
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
#include "../game/cands/cands.h"
#include "bench.h"

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

/**
 * Converts a double to 2 decimal places.
 */
std::string dp_2(double d) {
    std::string original = std::to_string(d);
    char characters[original.size() + 1];
    std::strcpy(characters, original.c_str());

    int i = 0;
    while (i < original.size() && characters[i] != '.') {
        ++i;
    }
    ++i;

    if (i + 1 >= original.size()) {
        characters[i + 2] = '\0';
        string str(&characters[0]);
        return str;
    }

    if (characters[i + 1] == '9' && characters[i + 2] >= '5') {
        // round up and carry
        if (characters[i] < '9') {
            // does not overflow decimal point
            ++characters[i];
            characters[i + 1] = '0';
        } else {
            // does overflow
            characters[i - 1] = '\0';
            string str(&characters[0]);
            int num = std::stoi(str);  // to int
            ++num;
            str = std::to_string(num);  // to string
            return str + ".00";
        }
    } else if (characters[i + 2] >= '5') {
        // round up
        ++characters[i + 1];
    }

    characters[i + 2] = '\0';
    string str(&characters[0]);
    return str;

}

/**
 * Calls a function with each piece of data provided, and averages the latency returned by
 * that function. Prints out the average time neatly, using the name provided.
 *
 * @param name the name to display
 * @param data a pointer to the first item of data
 * @param n the number of pieces of data
 * @param caller a function which takes some data, and returns the time taken to process it
 */
template<typename T, typename V>
double bench(std::string name, BenchUnit u, const T * data, int n, V (*func)(const T &)) {

    // start timer
    auto start = high_resolution_clock::now();

    int meaningless_value = 0;
    for (int i = 0; i < n; ++i) {
        meaningless_value += (*func)(data[i]);
    }

    // end benchmark and return
    auto stop = high_resolution_clock::now();

    long duration;

    switch (u) {
        case NANOS:
            duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();
            break;
        case MICROS:
        default:
            duration = duration_cast<std::chrono::microseconds>(stop - start).count();
            break;
        case MILLIS:
            duration = duration_cast<std::chrono::milliseconds>(stop - start).count();
            break;
        case SECONDS:
            duration = duration_cast<std::chrono::seconds>(stop - start).count();
            break;
    }

    double per_item = ((double) duration) / ((double) n);

    cout << std::left << setw(20) << name << ": "
         << std::right << setw(8) << dp_2(per_item)
         << " " << unit_name(u) << ((meaningless_value > 0) ? "\n" : " \n");

    return per_item;

}


double bench_metric(Metric *m, const string & name) {

    vector<string> vec;
    read_all_fens(vec);

    // initialise all boards
    Board * states = new Board[vec.size()];
    for (int i = 0; i < vec.size(); ++i) {
        states[i]= fen_to_board(vec[i]);
    }

    double time = bench(name, MICROS, states, vec.size(), m);

    delete[] states;

    return time;

}

void bench_individual_metrics() {
    cout << "Benchmarking individual metrics\n";
    double total_time = 0.0;
    for (int i = 0; i < METRICS_IN_USE; ++i) {
        // if (METRIC_NAMES[i] == "Centre control")
        total_time += bench_metric(METRICS[i], METRIC_NAMES[i]);
    }
    cout << std::left << setw(20) << "Total" << ": "
         << std::right << setw(8) << dp_2(total_time)
         << " " << unit_name(MICROS) << "\n";
}

int bench_game() {

    bench_individual_metrics();
    // heuristic_evaluation();

    return 0;

}

