#include <iostream>
using std::cout;
#include <cstring>
using std::strcmp;
#include <iomanip>
#include <csignal>

using std::setw;

#include "test.h"


int main(int argc, char** argv) {

    signal(SIGSEGV, print_stack_trace_and_abort);
    signal(SIGABRT, print_stack_trace_and_abort);

    unsigned modules_tested = 0;
    bool passed = true;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "-board") == 0) {
            cout << "\nTesting board\n";
            passed = test_board() && passed;
            ++modules_tested;
        } else if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "-game") == 0) {
            cout << "\nTesting game\n";
            passed = test_game() && passed;
            ++modules_tested;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "-search") == 0) {
            cout << "\nTesting search\n";
            passed = test_search() && passed;
            ++modules_tested;
        } else if (strcmp(argv[i], "-stress") == 0) {
            cout << "\nRunning stress tests\n";
            passed = stress_test_main() && passed;
            ++modules_tested;
        } else {
            cout << "Unrecognised argument: " + std::string(argv[i]) + "\n";
        }
    }

    if (modules_tested == 0) {
        cout << "No tests requested\n";
    } else if (!passed) {
        cout << "\n*****SOME TESTS FAILED*****\n";
    } else {
        cout << "\nAll tests passed!\n";
    }
    cout << "\nGoodbye\n";

    return passed ? 0 : 1;

}
