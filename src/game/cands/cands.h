#ifndef STASE_CANDS_H
#define STASE_CANDS_H

constexpr inline int MAX_MOVES_PER_HOOK() { return 3; }

typedef struct {
    const Square centre;
    const Square secondary;
} FeatureFrame;

typedef Move MoveSet[MAX_MOVES_PER_HOOK()];

typedef void (Hook)(const Board &, FeatureFrame*);
typedef void (Responder)(const Board &, MoveSet*, FeatureFrame*);

typedef struct {
    Hook* hook;
    Responder* responder;
} FeatureHandler;

extern FeatureHandler feature_handlers[];

int capture_walk(const Board &, Square, int*, int*);

Hook weak_hook;
Responder weak_resp;

#endif //STASE_CANDS_H
