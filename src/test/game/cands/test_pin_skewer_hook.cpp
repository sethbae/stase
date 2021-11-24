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

const TestSet<HookTestCase> pin_skewer_hook_test_set{
    "game-cands-pin-skewer-hook",
    {
        HookTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {}
        },
        // basic pin: bishop
        HookTestCase{
            "8/3B4/8/3K2B1/8/8/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"g5", "d5", 1, 0},
                ExpectedFeatureFrame{"d7", "d5", 0, 1},
                ExpectedFeatureFrame{"d5", "g5", -1, 0},
                ExpectedFeatureFrame{"d5", "d7", 0, -1},
            }
        },
        // basic pin: knight
        HookTestCase{
            "8/8/5kn1/4n3/8/8/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"e5", "f6", -1, -1},
                ExpectedFeatureFrame{"g6", "f6", 1, 0},
                ExpectedFeatureFrame{"f6", "e5", 1, 1},
                ExpectedFeatureFrame{"f6", "g6", -1, 0}
            }
        },
        // basic pin: rook
        HookTestCase{
            "8/8/1kr5/8/8/2R5/1K6/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"c3", "b2", 1, 1},
                ExpectedFeatureFrame{"b2", "c3", -1, -1},
                ExpectedFeatureFrame{"b6", "c6", -1, 0}
            }
        },
        // basic pin: queen
        HookTestCase{
            "8/8/1k6/1q6/8/3R4/1K6/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"b5", "b6", 0, -1},
                ExpectedFeatureFrame{"b6", "b5", 0, 1},
            }
        },
        // basic pin: queen #2
        HookTestCase{
            "8/8/8/3Q4/4K3/8/8/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"d5", "e4", -1, 1},
                ExpectedFeatureFrame{"e4", "d5", 1, -1},
            }
        },
        //basic pin: pawn
        HookTestCase{
            "8/8/3K4/8/5P2/8/3PN3/8 w - - 0 1",
            {
                ExpectedFeatureFrame{"d6", "d2", 0, 1},
                ExpectedFeatureFrame{"d2", "d6", 0, -1},
                ExpectedFeatureFrame{"d2", "e2", -1, 0},
                ExpectedFeatureFrame{"e2", "d2", 1, 0}
            }
        },
        // basic pin: not pawn
        HookTestCase{
            "6k1/5ppp/8/8/8/8/5PPP/6K1 w - - 0 1",
            {}
        },
        // basic pin: not bishop
        HookTestCase{
            "6k1/5bpp/8/8/8/8/5BPP/6K1 w - - 0 1",
            {}
        },
        // basic pin: not rook
        HookTestCase{
            "1r6/k2r4/8/8/1r6/8/3R4/3K4 w - - 0 1",
            {}
        },
        // puzzle #1
        HookTestCase{
            "r5k1/1bp4p/1p3bp1/3q1p2/4p3/4P2P/3QBPPB/2R3K1 w - - 0 27",
            {
                ExpectedFeatureFrame{"d5", "g8", -1, -1},
                ExpectedFeatureFrame{"d2", "e2", -1, 0},
                ExpectedFeatureFrame{"g1", "c1", 1, 0},
                ExpectedFeatureFrame{"g8", "a8", 1, 0}
            }
        }
    }
};

bool evaluate_pin_skewer_hook_test_case(const HookTestCase * tc) {

    Gamestate gs(fen_to_board(tc->fen));

    discover_feature_frames(gs, &pin_skewer_hook);

    int num_features = 0;

    for (FeatureFrame * ff = gs.feature_frames[pin_skewer_hook.id]; !is_sentinel(ff->centre); ++ff) {

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

bool test_pin_skewer_hook() {
    return evaluate_test_set(&pin_skewer_hook_test_set, &evaluate_pin_skewer_hook_test_case);
}
