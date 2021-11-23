#include "../../test.h"

/**
 * This holds strings instead of squares for ease of test-case entry.
 */
struct ExpectedFeatureFrame {
    const std::string centre;
    const std::string secondary;
    const int conf_1;
    const int conf_2;
};

struct HookTestCase {
    const std::string fen;
    std::vector<ExpectedFeatureFrame> expected_frames;
};

bool expected_frame_matches_actual(const ExpectedFeatureFrame & expected, const FeatureFrame & actual) {
    return equal(stosq(expected.centre), actual.centre)
            && equal(stosq(expected.secondary), actual.secondary)
            && expected.conf_1 == actual.conf_1
            && expected.conf_2 == actual.conf_2;
}

const TestSet<HookTestCase> kpinnable_test_set{
    "game-cands-kpinnable",
    {
        HookTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {
                ExpectedFeatureFrame{"d1", "e1", -1, 0},
                ExpectedFeatureFrame{"f1", "e1", 1, 0},
                ExpectedFeatureFrame{"d8", "e8", -1, 0},
                ExpectedFeatureFrame{"f8", "e8", 1, 0},
            }
        }
    }
};

bool evaluate_kpinnable_test_case(const HookTestCase * tc) {

    Gamestate gs(fen_to_board(tc->fen));

    discover_feature_frames(gs, &kpinnable_hook);

    int num_features = 0;

    for (FeatureFrame * ff = gs.feature_frames[kpinnable_hook.id]; !is_sentinel(ff->centre); ++ff) {

        bool found = false;
        for (const ExpectedFeatureFrame & expected : tc->expected_frames) {
            if (expected_frame_matches_actual(expected, *ff)) {
                found = true;
                break;
            }
        }

        if (!found) {
            return false;
        }
        ++num_features;
    }

    return num_features == tc->expected_frames.size();
}

bool test_kpinnable_hook() {
    return evaluate_test_set(&kpinnable_test_set, &evaluate_kpinnable_test_case);
}
