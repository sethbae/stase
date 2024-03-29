#ifndef STASE_RESPONDER_HPP
#define STASE_RESPONDER_HPP

#include "cands.h"

struct Responder {
    const std::string name;
    int (*resp)(const Gamestate &, const FeatureFrame *, Move *, int starting_index, int max_allowed_index);
};

extern const Responder defend_centre_resp;
extern const Responder defend_secondary_resp;
extern const Responder capture_resp;
extern const Responder develop_resp;
extern const Responder play_fork_resp;
extern const Responder trade_resp;
extern const Responder retreat_resp;
extern const Responder desperado_resp;
extern const Responder play_check_resp;
extern const Responder pin_skewer_resp;
extern const Responder promotion_resp;
extern const Responder play_discovered_resp;
extern const Responder approach_kings_resp;
extern const Responder avoid_mate_resp;

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
    &promotion_resp,
    &play_discovered_resp,
    &approach_kings_resp,
    &avoid_mate_resp
};

#endif //STASE_RESPONDER_HPP
