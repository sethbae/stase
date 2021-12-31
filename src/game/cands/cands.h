#ifndef STASE_CANDS_H
#define STASE_CANDS_H

#include "game.h"
#include "board.h"
#include "score.h"

const int MAX_TOTAL_CANDS = 30;
const int MAX_MOVES_PER_HOOK = 10;
const int MAX_MOVES_PER_FRAME = 5;

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
    void (*hook)(Gamestate &, const Square centre, std::vector<FeatureFrame> & frames);
};

struct Responder {
    const std::string name;
    void (*resp)(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
};

void discover_feature_frames(Gamestate &, const Hook *);
void record_hook_features(Gamestate &, const Hook *, FeatureFrame *, int);

struct FeatureHandler {
    const Hook * hook;
    const std::vector<const Responder *> friendly_responses;
    const std::vector<const Responder *> enemy_responses;
};

void is_unsafe_piece_hook(Gamestate &, const Square, std::vector<FeatureFrame> &);
void is_undeveloped_piece_hook(Gamestate &, const Square, std::vector<FeatureFrame> &);
void find_forks_hook(Gamestate &, const Square, std::vector<FeatureFrame> &);
void find_checks_hook(Gamestate &, const Square, std::vector<FeatureFrame> &);
void find_pin_skewer_hook(Gamestate &, const Square, std::vector<FeatureFrame> &);
void identify_king_pinned_pieces_hook(Gamestate &, const Square, std::vector<FeatureFrame> &);
void can_promote_hook(Gamestate &, const Square, std::vector<FeatureFrame> &);

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

// the actual hooks
const Hook unsafe_piece_hook = Hook{"unsafe-piece", 0, &is_unsafe_piece_hook};
const Hook develop_hook = Hook{"development", 1, &is_undeveloped_piece_hook};
const Hook fork_hook = Hook{"fork", 2, &find_forks_hook};
const Hook check_hook = Hook{"check", 3, &find_checks_hook};
const Hook pin_skewer_hook = Hook{"pin-skewer", 4, &find_pin_skewer_hook};
const Hook king_pinned_pieces_hook = Hook{"king-pinned-piece", 5, &identify_king_pinned_pieces_hook};
const Hook promotion_hook = Hook{"promotion", 6, &can_promote_hook};

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
            { &retreat_resp, &trade_resp },
            { &pin_skewer_resp }
        },
        FeatureHandler{
                &develop_hook,
                { &develop_resp },
                { }
        }
};

#endif //STASE_CANDS_H
