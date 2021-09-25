#ifndef STASE_BENCH_H
#define STASE_BENCH_H

#include <chrono>

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


int bench_board();

int bench_game();

#endif //STASE_BENCH_H
