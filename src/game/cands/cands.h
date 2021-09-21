#ifndef STASE_CANDS_H
#define STASE_CANDS_H

#include "board.h"

const int MAX_TOTAL_CANDS = 10;
const int MAX_MOVES_PER_HOOK = 5;
const int MAX_RESPONDERS_PER_HOOK = 5;
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
    inline void clear_soft_limit(int limit) {
        soft_limit = 0;
    }
};

typedef bool (Hook)(const Board &, Square centre, Square * secondary, int * conf_1, int * conf_2);
typedef void (Responder)(const Board &, const FeatureFrame *, Move *, IndexCounter & move_counter);

void discover_feature_frames(const Board &, Hook *, FeatureFrame **);

struct FeatureHandler {
    Hook * hook;
    Responder * friendly_responses[MAX_RESPONDERS_PER_HOOK + 1];
    Responder * enemy_responses[MAX_RESPONDERS_PER_HOOK + 1];
};

Hook weak_hook;
Hook development_hook;
Responder defend_square;
Responder capture_piece;
Responder develop_piece;

const FeatureHandler feature_handlers[] = {
        FeatureHandler{
          &development_hook,
          { &develop_piece, nullptr },
          { nullptr }
        },
        FeatureHandler{
            &weak_hook,
            { &defend_square, nullptr },
            { &capture_piece, nullptr }
        },
        FeatureHandler{  // sentinel
            nullptr
        }
};

#endif //STASE_CANDS_H
