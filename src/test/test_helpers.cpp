#include <cstring>
#include "test.h"

bool evaluate_responder_test_case(const Responder * resp, const ResponderTestCase * tc) {

    Gamestate gs(fen_to_board(tc->fen));
    Move moves[MAX_MOVES_PER_HOOK];
    IndexCounter move_counter(MAX_MOVES_PER_HOOK);

    // run the responder on the feature frames
    for (FeatureFrame ff : tc->feature_frames) {
        resp->resp(gs, &ff, &moves[0], move_counter);
    }

    // convert the output to a vector of strings
    std::vector<std::string> strings;
    for (int i = 0; i < move_counter.idx(); ++i) {
        strings.push_back(sqtos(moves[i].from) + sqtos(moves[i].to));
    }

    // check that they match
    return assert_string_lists_equal(strings, tc->expected_results);
}

bool evaluate_hook_test_case(const Hook * h, const HookTestCase * tc) {

    Gamestate gs(fen_to_board(tc->fen));

    discover_feature_frames(gs, h);

    int num_features = 0;

    for (FeatureFrame * ff = gs.feature_frames[h->id]; !is_sentinel(ff->centre); ++ff) {

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

/**
 * Checks that all strings present in one vector are present in the other, and no more.
 */
bool assert_string_lists_equal(const std::vector<std::string> & a, const std::vector<std::string> & b) {

    if (a.size() != b.size()) { return false; }

    for (const std::string stra : a) {
        bool present = false;
        for (const std::string strb : b) {
            if (std::strcmp(stra.c_str(), strb.c_str()) == 0) {
                present = true;
                break;
            }
        }
        if (!present) {
            return false;
        }
    }

    return true;

}

/**
 * Compares an ExpectedFeatureFrame to an actual one, unpacking strings and checking the squares match.
 */
bool expected_frame_matches_actual(const ExpectedFeatureFrame & expected, const FeatureFrame & actual) {
    return equal(stosq(expected.centre), actual.centre)
           && equal(stosq(expected.secondary), actual.secondary)
           && expected.conf_1 == actual.conf_1
           && expected.conf_2 == actual.conf_2;
}

/**
 * Converts for example "e2e4" into a Move from the e2 square to the e4 square.
 * If the string is not valid, then MOVE_SENTINEL will be returned.
 */
Move unpack_four_char_san(const std::string & s) {
    if (s.size() != 4) {
        // invalid string
        return MOVE_SENTINEL;
    }
    return Move{
            stosq(s.substr(0, 2)),
            stosq(s.substr(2, 4))
    };
}
