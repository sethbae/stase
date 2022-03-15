#ifndef STASE_FEATURES_HPP
#define STASE_FEATURES_HPP

#include "cands.h"
#include "hook.hpp"
#include "responder.hpp"

struct FeatureHandler {
    const Hook & hook;
    const std::vector<const Responder *> friendly_responses;
    const std::vector<const Responder *> enemy_responses;
};

const std::vector<FeatureHandler> feature_handlers = {
    FeatureHandler{
        king_pinned_pieces_hook,
        { &defend_centre_resp },
        { &capture_resp, &defend_centre_resp }
    },
    FeatureHandler{
        promotion_hook,
        { &promotion_resp, &defend_centre_resp },
        { &defend_centre_resp }
    },
    FeatureHandler{
        check_hook,
        { &play_check_resp },
        { }
    },
    FeatureHandler{
        unsafe_piece_hook,
        { &trade_resp, &retreat_resp, &defend_centre_resp, &desperado_resp },
        { &capture_resp }
    },
    FeatureHandler{
        fork_hook,
        { &defend_secondary_resp },
        { &play_fork_resp }
    },
    FeatureHandler{
        queen_fork_hook,
        { &defend_secondary_resp },
        { &play_fork_resp }
    },
    FeatureHandler{
        pin_skewer_hook,
        { &pin_skewer_resp },
        { &defend_secondary_resp },
    },
    FeatureHandler{
        discovered_hook,
        { &play_discovered_resp },
        { }
    },
    FeatureHandler{
        positional_hook,
        { &trade_resp, &retreat_resp },
        { &trade_resp, &retreat_resp }
    },
    FeatureHandler{
        develop_hook,
        { &develop_resp },
        { }
    }
};

#endif //STASE_FEATURES_HPP
