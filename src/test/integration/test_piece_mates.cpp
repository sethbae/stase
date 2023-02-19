#include "integration_test.h"

const TestSet<IntegrationTest> piece_mates_test_set{
    "integration-piece-mates",
    {}
};

bool test_piece_mates() {
    return evaluate_test_set(&piece_mates_test_set, &evaluate_integration_test);
}
