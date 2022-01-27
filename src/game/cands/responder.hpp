#ifndef STASE_RESPONDER_HPP
#define STASE_RESPONDER_HPP

#include "cands.h"

struct Responder {
    const std::string name;
    void (*resp)(const Gamestate &, const FeatureFrame *, Move *, IndexCounter &);
};

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

#endif //STASE_RESPONDER_HPP
