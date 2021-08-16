#include <iostream>
using std::cout;
#include <cstring>
using std::strcmp;

#include "bench.h"

int main(int argc, char** argv) {

    unsigned benchmarks_run = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "-board") == 0) {
            cout << "Benching board\n";
            bench_board();
            ++benchmarks_run;
        } else if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "-game") == 0) {
            cout << "Benching game\n";
            bench_game();
            ++benchmarks_run;
        } else {
            cout << "Unrecognised argument: " + std::string(argv[i]) + "\n";
        }
    }

    if (benchmarks_run == 0) {
        cout << "No benchmarks requested\n";
    }
    cout << "Goodbye\n";

    return 0;
}
