#ifndef STASE_CANDS_H
#define STASE_CANDS_H

#include "board.h"

const int MAX_TOTAL_CANDS = 10;
const int MAX_MOVES_PER_HOOK = 5;
const int MAX_MOVES_PER_RESPONDER = 5;
const int MAX_RESPONDERS_PER_HOOK = 5;
extern const int NUM_FEATURES;

struct FeatureFrame {
    Square centre;
    Square secondary;
    int conf_1;
    int conf_2;
};

struct MoveSet {
    Move moves[MAX_MOVES_PER_HOOK];
};

typedef void (Hook)(const Board &, FeatureFrame**);
typedef void (Responder)(const Board &, const FeatureFrame*, MoveSet*, int & move_counter);

struct FeatureHandler {
    Hook* hook;
    Responder* friendly_responses[MAX_RESPONDERS_PER_HOOK + 1];
    Responder* enemy_responses[MAX_RESPONDERS_PER_HOOK + 1];
};

Hook weak_hook;
Responder defend_square;
Responder capture_piece;

const FeatureHandler feature_handlers[] = {
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
