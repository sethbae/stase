#ifndef STASE_CANDS_H
#define STASE_CANDS_H

const int MAX_MOVES_PER_HOOK = 3;
const int MAX_RESPONSES_PER_HOOK = 3;
extern const int NUM_FEATURES;

struct FeatureFrame {
    Square centre;
    Square secondary;
    int conf_1;
    int conf_2;
};

typedef struct {
    Move moves[MAX_MOVES_PER_HOOK];
} MoveSet;

typedef void (Hook)(const Board &, FeatureFrame**);
typedef void (Responder)(const Board &, FeatureFrame*, MoveSet*, int & move_counter);

typedef struct {
    Hook* hook;
    Responder* friendly_responses[MAX_RESPONSES_PER_HOOK + 1];
    Responder* enemy_responses[MAX_RESPONSES_PER_HOOK + 1];
} FeatureHandler;

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
