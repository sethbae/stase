#ifndef STASE_FORK_HELPERS_H
#define STASE_FORK_HELPERS_H

#include <string>
#include <vector>

struct ForkFrame {
    const std::string forked_a;
    const std::string forked_b;
    const std::string fork_sq;
    const std::string from_sq;
};

struct ForkTestCase {
    const std::string fen;
    const std::vector<ForkFrame> expected_frames;
};

inline bool fork_frames_equivalent(const ForkFrame & expected, const FeatureFrame & actual) {

    if ((expected.forked_a == sqtos(actual.centre) && expected.forked_b == sqtos(actual.secondary))
        || (expected.forked_b == sqtos(actual.centre) && expected.forked_a == sqtos(actual.secondary))) {
        if (sqtos(itosq(actual.conf_1)) == expected.fork_sq
            && sqtos(itosq(actual.conf_2)) == expected.from_sq) {
            return true;
        }
    }
    return false;
}

inline bool list_contains_match(const std::vector<ForkFrame> expecteds, const FeatureFrame & actual) {
    for (const ForkFrame & expected : expecteds) {
        if (fork_frames_equivalent(expected, actual)) {
            return true;
        }
    }
    return false;
}

inline bool evaluate_fork_test_case(const ForkTestCase * tc, const Ptype expected) {

    Gamestate gs(tc->fen);

    discover_feature_frames(gs, &fork_hook);

    std::vector<std::string> strings;

    int actual_frames_encountered = 0;

    for (int i = 0; !is_sentinel(gs.frames[fork_hook.id][i].centre) && i < MAX_FRAMES; ++i) {

        FeatureFrame ff = gs.frames[fork_hook.id][i];
        if (ff.conf_2 == sq_sentinel_as_int()) {
            continue;
        }
        if (type(gs.board.get(itosq(ff.conf_2))) != expected) {
            continue;
        }

        ++actual_frames_encountered;
        if (!list_contains_match(tc->expected_frames, ff)) {
            return false;
        }
    }

    return tc->expected_frames.size() == actual_frames_encountered;
}

#endif //STASE_FORK_HELPERS_H
