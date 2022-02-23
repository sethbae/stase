#ifndef STASE_HOOK_HPP
#define STASE_HOOK_HPP

#include "cands.h"

struct Hook {
    const std::string name;
    const int id;
    bool (*hook)(Gamestate &, const Square centre);
};

void discover_feature_frames(Gamestate &, const Hook *);

extern const Hook unsafe_piece_hook;
extern const Hook develop_hook;
extern const Hook fork_hook;
extern const Hook queen_fork_hook;
extern const Hook check_hook;
extern const Hook pin_skewer_hook;
extern const Hook king_pinned_pieces_hook;
extern const Hook promotion_hook;

const std::vector<const Hook *> ALL_HOOKS {
        &king_pinned_pieces_hook,
        &unsafe_piece_hook,
        &develop_hook,
        &fork_hook,
        &queen_fork_hook,
        &check_hook,
        &pin_skewer_hook,
        &promotion_hook
};

// in order to allocate the feature frames array statically, we cannot take the value
// from the vector dynamically. Instead we have to do this :(
const int NUM_HOOKS = 8;
inline bool assert_failed() {
    std::cout << "Assertion failed: update NUM_HOOKS to correctly match the number of hooks\n";
    abort();
    return true;
}
const bool assertion_failed = (NUM_HOOKS == ALL_HOOKS.size() ? false : assert_failed());

#endif //STASE_HOOK_HPP
