#ifndef STASE_RESPONDER_HPP
#define STASE_RESPONDER_HPP

#include "cands.h"

struct Responder {
    const std::string name;
    void (*resp)(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
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
        &play_discovered_resp
};

#endif //STASE_RESPONDER_HPP
