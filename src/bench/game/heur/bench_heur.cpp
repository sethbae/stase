#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <iomanip>
using std::setw;

#include "game.h"
#include "puzzle.h"
#include "../../../game/heur/heur.h"
#include "../../bench.h"

void bench_heuristic_evaluation() {

    cout << "Benching heuristic evaluation\n";

    vector<string> vec;
    retrieve_all_puzzle_fens(vec);

    // initialise all boards
    Gamestate *states = new Gamestate[vec.size()];
    for (int i = 0; i < vec.size(); ++i) {
        states[i].board = fen_to_board(vec[i]);
    }

    bench("heur", MICROS, states, vec.size(), heur);

    delete[] states;

}

double bench_metric(Metric *m, const string &name) {

    vector<string> vec;
    read_all_fens(vec);

    // initialise all boards
    Board *states = new Board[vec.size()];
    for (int i = 0; i < vec.size(); ++i) {
        states[i] = fen_to_board(vec[i]);
    }

    double time = bench(name, MICROS, states, vec.size(), m);

    delete[] states;

    return time;

}

void bench_individual_metrics() {

    cout << "Benchmarking individual metrics\n";

    double total_time = 0.0;
    for (int i = 0; i < METRICS_IN_USE; ++i) {
        total_time += bench_metric(METRICS[i], METRIC_NAMES[i]);
    }

    cout << std::left << setw(20) << "Total" << ": "
         << std::right << setw(8) << dp_2(total_time)
         << " " << unit_name(MICROS) << "\n";
}
