#ifndef STASE_TEST_H
#define STASE_TEST_H

#include <vector>
#include <iostream>
#include <iomanip>

struct StringTestCase {
    const std::string fen;
    const std::vector<std::string> expected_results;
};

struct StringTestSet {
    const std::string name;
    const std::vector<StringTestCase> cases;
};

inline bool evaluate_test_set(const StringTestSet * test_set, bool (*func)(const StringTestCase*)) {

    if (test_set->cases.empty()) { return true; }

    int success = 0, failed = 0, first_failure = -1;

    for (StringTestCase tc : test_set->cases) {
        if ((*func)(&tc)) {
            ++success;
        } else {
            ++failed;
            first_failure =
                    (first_failure < 0)
                    ? success
                    : first_failure;
        }
    }

    std::cout << std::setw(20) << test_set->name << ":   ";
    std::cout << success << "/" << success + failed << "\n";

    if (failed) {
        std::cout << "*****SOME TESTS FAILED***** ("
                    << test_set->name << "@" << first_failure << ")\n";
        return false;
    }

    return true;

}

bool assert_string_lists_equal(const std::vector<std::string> &, const std::vector<std::string> &);

void test_board();
void test_game();

void test_weak_hook();
void test_weak_cap();

#endif //STASE_TEST_H
