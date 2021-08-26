#include <iostream>
using std::cout;
#include <cstring>
using std::strcmp;
#include <iomanip>
using std::setw;

#include "test.h"


int main(int argc, char** argv) {

    unsigned modules_tested = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "-board") == 0) {
            cout << "Testing board\n";
            test_board();
            ++modules_tested;
        } else if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "-game") == 0) {
            cout << "Testing game\n";
            test_game();
            ++modules_tested;
        } else {
            cout << "Unrecognised argument: " + std::string(argv[i]) + "\n";
        }
    }

    if (modules_tested == 0) {
        cout << "No tests requested\n";
    }
    cout << "Goodbye\n";

    return 0;

}
