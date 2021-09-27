#ifndef STASE_CANDS_H
#define STASE_CANDS_H

#include "game.h"
#include "board.h"

const int MAX_TOTAL_CANDS = 10;
const int MAX_MOVES_PER_HOOK = 5;

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

struct Hook {
    const std::string name;
    const int id;
    bool (*hook)(const Gamestate &, Square centre, FeatureFrame * ff);
};

struct Responder {
    const std::string name;
    const int id;
    void (*resp)(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
};

void discover_feature_frames(const Gamestate &, const Hook *);

struct FeatureHandler {
    const Hook * hook;
    const std::vector<const Responder *> friendly_responses;
    const std::vector<const Responder *> enemy_responses;
};

// functions used by hooks
bool is_weak_square(const Gamestate &, Square, FeatureFrame *);
bool is_undeveloped_piece(const Gamestate &, Square, FeatureFrame *);

// functions used by responders
void defend_square(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void capture_piece(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void develop_piece(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);

// the actual hooks
const Hook weak_hook = Hook{"weak", 0, &is_weak_square};
const Hook develop_hook = Hook{"development", 1, &is_undeveloped_piece};

// the actual responders
const Responder defend_resp = Responder{"defend", 0, &defend_square};
const Responder capture_resp = Responder{"capture", 0, &capture_piece};
const Responder develop_resp = Responder{"develop", 0, &develop_piece};

const std::vector<Hook> ALL_HOOKS {
        weak_hook,
        develop_hook
};

const std::vector<FeatureHandler> feature_handlers = {
        FeatureHandler{
          &develop_hook,
          { &develop_resp },
          { }
        },
        FeatureHandler{
            &weak_hook,
            { &defend_resp },
            { &capture_resp }
        }
};

#endif //STASE_CANDS_H
