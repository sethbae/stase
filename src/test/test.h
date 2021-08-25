#ifndef STASE_TEST_H
#define STASE_TEST_H

#include <vector>

class TestCase {
    int seth;
    virtual ~TestCase();
};

void evaluate_test_set(std::vector<TestCase>, bool (*func)(TestCase));

void test_board();
void test_game();

#endif //STASE_TEST_H
