#ifndef STASE_BENCH_H
#define STASE_BENCH_H

#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
#include <iostream>
using std::cout;
#include <iomanip>
#include <vector>

#include "../utils/utils.h"
#include "game.h"
#include "board.h"

enum BenchUnit {
    NANOS,
    MICROS,
    MILLIS,
    SECONDS
};

inline std::string unit_name(BenchUnit u) {
    switch (u) {
        case NANOS:
            return "ns";
        case MICROS:
            return "us";
        case MILLIS:
            return "ms";
        case SECONDS:
            return "s";
        default:
            return "unknown unit";
    }
}

/**
 * Calls a function with each piece of data provided, and averages the latency returned by
 * that function. Prints out the average time neatly, using the name provided.
 *
 * @tparam T the type of the data
 * @tparam V the return type of the given function (should be numeric)
 * @param name the name to display
 * @param u the time unit to measure
 * @param data a pointer to the first item of data
 * @param n the number of pieces of data
 * @param func a function which takes some data, and returns the time taken to process it
 * @return the number of units the function took per item.
 */
template<typename T, typename V>
inline double bench(const std::string & name, BenchUnit u, const T * data, int n, V (*func)(const T &)) {

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

    cout << std::left << std::setw(20) << name << ": "
         << std::right << std::setw(8) << dp_2(per_item)
         << " " << unit_name(u) << ((meaningless_value > 0) ? "\n" : " \n");

    return per_item;

}

int bench_board();
int bench_game();

void bench_individual_metrics();
void bench_heuristic_evaluation();

void bench_individual_hooks();
void bench_individual_responders();
void bench_cands();

#endif //STASE_BENCH_H
