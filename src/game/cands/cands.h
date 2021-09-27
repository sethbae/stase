#ifndef STASE_CANDS_H
#define STASE_CANDS_H

#include "board.h"

const int MAX_TOTAL_CANDS = 10;
const int MAX_MOVES_PER_HOOK = 5;
extern const int NUM_FEATURES;

struct FeatureFrame {
    Square centre;
    Square secondary;
    int conf_1;
    int conf_2;
};

struct IndexCounter {
    int hard_limit;
    int soft_limit;
    int current_index;

    IndexCounter(int hard_limit) {
        this->hard_limit = hard_limit;
        this->current_index = 0;
        this->soft_limit = 0;
    }

    /**
     * Tries to increase the current number of items counted. If there is space,
     * then the current size is increased.
     *
     * @return true if there was space, false otherwise.
     */
    inline bool has_space() {
        int max = soft_limit ? soft_limit : hard_limit;
        return current_index < max;
    }

    inline int idx() { return current_index; }
    inline int inc() { return current_index++; }

    /**
     * Sets a new, temporary limit on the counter. Allows the index to increase
     * this far above its current value.
     *
     * @param limit the amount it can increase by.
     */
    inline void add_allowance(int limit) {
        soft_limit = current_index + limit;
    }

    /**
     * Clears any allowance, if some was previously given.
     * @param limit
     */
    inline void clear_soft_limit() {
        soft_limit = 0;
    }
};

typedef bool (Hook)(const Board &, Square centre, FeatureFrame * ff);
typedef void (Responder)(const Board &, const FeatureFrame *, Move *, IndexCounter & move_counter);

void discover_feature_frames(const Board &, Hook *, FeatureFrame **);

struct FeatureHandler {
    Hook * hook;
    const std::vector<Responder *> friendly_responses;
    const std::vector<Responder *> enemy_responses;
};

Hook weak_hook;
Hook development_hook;
Responder defend_square;
Responder capture_piece;
Responder develop_piece;

const std::vector<Hook *> ALL_HOOKS {
        &weak_hook,
        &development_hook
};

const std::vector<std::string> HOOK_NAMES {
        "weak-hook",
        "development-hook"
};

const std::vector<FeatureHandler> feature_handlers = {
        FeatureHandler{
          &development_hook,
          { &develop_piece },
          { }
        },
        FeatureHandler{
            &weak_hook,
            { &defend_square },
            { &capture_piece }
        }
};

#endif //STASE_CANDS_H
