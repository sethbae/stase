#ifndef STASE_TEST_H
#define STASE_TEST_H

#include <vector>
#include <iostream>
#include <iomanip>

template <typename T>
bool evaluate_test_set(std::vector<T> & cases, bool (*func)(const T*)) {

    if (cases.empty()) { return true; }

    int success = 0, failed = 0;

    for (T tc : cases) {
        if ((*func)(&tc)) {
            ++success;
        } else {
            ++failed;
        }
    }

    std::cout << std::setw(20) << cases[0].name() << ":   ";
    std::cout << success << "/" << success + failed << "\n";

    if (failed) {
        std::cout << "***SOME TESTS FAILED***\n";
        return false;
    }

    return true;

}

void test_board();
void test_game();

void test_weak();

#endif //STASE_TEST_H
