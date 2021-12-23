#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <iomanip>
using std::setw;

#include "game.h"
#include "../../../game/heur/heur.h"
#include "../../bench.h"
#include "puzzle.h"

void bench_heuristic_evaluation() {
    cout << "Benching heuristic evaluation\n";
    vector<Gamestate> gamestates;
    puzzle_gamestates(gamestates);
    bench("heur", MICROS, gamestates.data(), gamestates.size(), heur);
}

double bench_metric(const Metric *m) {
    vector<Gamestate> states;
    puzzle_gamestates(states);
    double time = bench(m->name, MICROS, states.data(), states.size(), m->metric);
    return time;
}

void bench_individual_metrics() {

    cout << "Benchmarking individual metrics\n";

    double total_time = 0.0;
    for (int i = 0; i < ALL_METRICS.size(); ++i) {
        total_time += bench_metric(ALL_METRICS[i]);
    }

    cout << std::left << setw(20) << "Total" << ": "
         << std::right << setw(8) << dp_2(total_time)
         << " " << unit_name(MICROS) << "\n";
}
