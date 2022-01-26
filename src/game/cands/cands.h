#ifndef STASE_CANDS_H
#define STASE_CANDS_H

#include "game.h"
#include "board.h"
#include "score.h"
#include <iostream>

const int MAX_TOTAL_CANDS = 30;
const int MAX_MOVES_PER_HOOK = 20;
const int MAX_MOVES_PER_FRAME = 5;

float quiess(const Gamestate &);

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
        if (soft_limit > hard_limit) { soft_limit = hard_limit; }
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
    bool (*hook)(Gamestate &, const Square centre);
};

extern const Hook unsafe_piece_hook;
extern const Hook develop_hook;
extern const Hook fork_hook;
extern const Hook check_hook;
extern const Hook pin_skewer_hook;
extern const Hook king_pinned_pieces_hook;
extern const Hook promotion_hook;

struct Responder {
    const std::string name;
    void (*resp)(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
};

void discover_feature_frames(Gamestate &, const Hook *);

struct FeatureHandler {
    const Hook * hook;
    const std::vector<const Responder *> friendly_responses;
    const std::vector<const Responder *> enemy_responses;
};

inline void print_feature_frames(const FeatureFrame * ff) {
    std::cout << "Printing feature-frame list @" << ff << ":\n";
    for (const FeatureFrame* ptr = ff; !equal(ptr->centre, SQUARE_SENTINEL); ptr++) {
        std::cout << "Frame: " << sqtos(ptr->centre) << " " << sqtos(ptr->secondary) << "\n";
    }
    std::cout << "(sentinel)\n";
}

// functions used by responders
void defend_centre(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void defend_secondary(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void capture_piece(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void develop_piece(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void play_fork(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void trade_piece(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void retreat_piece(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void desperado_piece(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void play_check(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void pin_or_skewer_piece(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
void promote_pawn(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);

// the actual responders
const Responder defend_centre_resp = Responder{"defend-centre", &defend_centre};
const Responder defend_secondary_resp = Responder{"defend-secondary", &defend_secondary};
const Responder capture_resp = Responder{"capture", &capture_piece};
const Responder develop_resp = Responder{"develop", &develop_piece};
const Responder play_fork_resp = Responder{"fork", &play_fork};
const Responder trade_resp = Responder{"trade", &trade_piece};
const Responder retreat_resp = Responder{"retreat", &retreat_piece};
const Responder desperado_resp = Responder{"desperado", &desperado_piece};
const Responder play_check_resp = Responder{"check", &play_check};
const Responder pin_skewer_resp = Responder{"pin-skewer", &pin_or_skewer_piece};
const Responder promotion_resp = Responder{"promotion", &promote_pawn};

const std::vector<const Hook *> ALL_HOOKS {
        &king_pinned_pieces_hook,
        &unsafe_piece_hook,
        &develop_hook,
        &fork_hook,
        &check_hook,
        &pin_skewer_hook,
        &promotion_hook
};

// in order to allocate the feature frames array statically, we cannot take the value
// from the vector dynamically. Instead we have to do this :(
const int NUM_HOOKS = 7;
inline bool assert_failed() { abort(); return true; }
const bool assertion_failed = (NUM_HOOKS == ALL_HOOKS.size() ? false : assert_failed());

const std::vector<const Responder *> ALL_RESPONDERS = {
        &defend_centre_resp,
        &defend_secondary_resp,
        &capture_resp,
        &develop_resp,
        &play_fork_resp,
        &trade_resp,
        &retreat_resp,
        &desperado_resp,
        &play_check_resp,
        &pin_skewer_resp,
        &promotion_resp
};

const std::vector<FeatureHandler> feature_handlers = {
        FeatureHandler{
          &king_pinned_pieces_hook,
          { &defend_centre_resp },
          { &capture_resp, &defend_centre_resp }
        },
        FeatureHandler{
            &promotion_hook,
            { &promotion_resp, &defend_centre_resp },
            { &defend_centre_resp }
        },
        FeatureHandler{
            &check_hook,
            { &play_check_resp },
            { }
        },
        FeatureHandler{
            &unsafe_piece_hook,
            { &trade_resp, &retreat_resp, &defend_centre_resp, &desperado_resp },
            { &capture_resp }
        },
        FeatureHandler{
            &fork_hook,
            { &play_fork_resp },
            { &defend_secondary_resp }
        },
        FeatureHandler{
            &pin_skewer_hook,
            { &pin_skewer_resp },
            { &defend_secondary_resp },
        },
        FeatureHandler{
                &develop_hook,
                { &develop_resp },
                { }
        }
};

#endif //STASE_CANDS_H
