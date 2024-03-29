#include <cstring>
#include <cstdlib>
#include "test.h"
#include "../game/gamestate.hpp"

bool evaluate_responder_test_case(const Responder * resp, const ResponderTestCase * tc) {

    Gamestate gs(tc->fen);
    Move moves[MAX_MOVES_PER_HOOK];
    int m = 0;

    discover_feature_frames(gs, king_pinned_pieces_hook);

    // run the responder on the feature frames
    for (FeatureFrame ff : tc->feature_frames) {
        m = resp->resp(gs, &ff, &moves[0], m, MAX_MOVES_PER_HOOK);
    }

    // convert the output to a vector of strings
    std::vector<std::string> strings;
    for (int i = 0; i < m; ++i) {
        strings.push_back(sqtos(moves[i].from) + sqtos(moves[i].to));
    }

    // check that they match
    return assert_string_lists_equal(strings, tc->expected_results);
}

bool evaluate_hook_test_case(const Hook & h, const HookTestCase * tc) {

    Gamestate gs(tc->fen);

    if (h.id != king_pinned_pieces_hook.id) {
        discover_feature_frames(gs, king_pinned_pieces_hook);
    }
    discover_feature_frames(gs, h);

    int num_features = 0;

    for (FeatureFrame * ff = gs.frames[h.id]; !is_sentinel(ff->centre); ++ff) {

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

    for (const std::string & stra : a) {
        bool present = false;
        for (const std::string & strb : b) {
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
    Square expected_centre = expected.centre == "sentinel" ? SQUARE_SENTINEL : stosq(expected.centre);
    Square expected_secondary = expected.secondary == "sentinel" ? SQUARE_SENTINEL : stosq(expected.secondary);
    return equal(expected_centre, actual.centre)
           && equal(expected_secondary, actual.secondary)
           && expected.conf_1 == actual.conf_1
           && expected.conf_2 == actual.conf_2;
}

/**
 * Returns a random but valid square.
 */
Square random_square() {
    return mksq(rand() % 8, rand() % 8);
}

/**
 * Returns a random move from any square to any square, with the flags set to zero.
 */
Move random_move() {
    return Move{random_square(), random_square(), 0};
}
