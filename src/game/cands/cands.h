#ifndef STASE_CANDS_H
#define STASE_CANDS_H

const int MAX_MOVES_PER_HOOK = 3;
extern const int NUM_FEATURES;

struct FeatureFrame {
    Square centre;
    Square secondary;
};

typedef struct {
    Move moves[MAX_MOVES_PER_HOOK];
} MoveSet;

typedef void (Hook)(const Board &, FeatureFrame*);
typedef void (Responder)(const Board &, MoveSet*, FeatureFrame*);

typedef struct {
    Hook* hook;
    Responder* responder;
} FeatureHandler;

extern FeatureHandler feature_handlers[];

Hook weak_hook;
Responder weak_resp;

#endif //STASE_CANDS_H
