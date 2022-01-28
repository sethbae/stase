#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <iomanip>
using std::setw;

#include "game.h"
#include "../../../game/cands/cands.h"
#include "../../../game/cands/responder.hpp"
#include "../../../game/cands/features.hpp"
#include "../../../game/gamestate.hpp"
#include "../../bench.h"
#include "puzzle.h"

/* bench hooks individually */

struct BenchHookParam {
    const Hook * h;
    Gamestate & gs;
};

bool run_hook(const BenchHookParam & b_h_p) {

    discover_feature_frames(b_h_p.gs, b_h_p.h);

    // count the feature frames and return that
    int frame_count = 0;
    for (int i = 0; !is_sentinel(b_h_p.gs.frames[b_h_p.h->id][i].centre); ++i, ++frame_count);

    return frame_count;
}

void bench_hook(const Hook * h) {

    vector<Gamestate> states;
    puzzle_gamestates(states);

    vector<BenchHookParam> params;
    for (Gamestate & gs : states) {
        params.push_back(BenchHookParam{h, gs});
    }

    bench(h->name, MICROS, params.data(), params.size(), &run_hook);

}

void bench_individual_hooks() {

    cout << "Benching individual hooks\n";

    for (int i = 0; i < ALL_HOOKS.size(); ++i) {
        bench_hook(ALL_HOOKS[i]);
    }
}

/* bench the individual responders */

struct BenchResponderParam {
    const Responder * r;
    const Gamestate * gs;
    const FeatureFrame * ff;
    Move * m;
};

int wrap_responder(const BenchResponderParam & params) {
    IndexCounter counter(MAX_TOTAL_CANDS);
    params.r->resp(*params.gs, params.ff, params.m, counter);
    return counter.idx();
}

void bench_responder(std::vector<Gamestate> & gamestates, const Responder * resp) {

    std::vector<BenchResponderParam> resp_params;

    Move ignored_moves[MAX_TOTAL_CANDS];

    for (FeatureHandler fh : feature_handlers) {

        // check if the feature handler contains this responder
        bool contains = false;
        for (const Responder * r : fh.enemy_responses) {
            if (r == resp) {
                contains = true;
            }
        }
        for (const Responder * r : fh.friendly_responses) {
            if (r == resp) {
                contains = true;
            }
        }

        if (!contains) { continue; }

        // run the hook on the gamestate, and add each feature frame to the params list
        for (Gamestate & gs: gamestates) {
            discover_feature_frames(gs, fh.hook);

            for (int i = 0; !is_sentinel(gs.frames[fh.hook->id][i].centre); ++i) {
                resp_params.push_back(
                        BenchResponderParam{
                            resp,
                            &gs,
                            &gs.frames[fh.hook->id][i],
                            ignored_moves
                        }
                );
            }
        }
    }

    // cout << resp->name << ": " << resp_params.size() << " frames\n";

    bench(resp->name, MICROS, resp_params.data(), resp_params.size(), &wrap_responder);

}

void bench_individual_responders() {

    cout << "Benching individual responders\n";

    std::vector<Gamestate> gamestates;
    puzzle_gamestates(gamestates);

    for (const Responder * r : ALL_RESPONDERS) {
        bench_responder(gamestates, r);
    }

}

/* bench the whole candidates function */

int wrap_cands(Gamestate & gs) {
    return cands(gs)->critical.size();
}
//
//int piece_fork(const Gamestate & gs) {
//
//    find_sliding_forks(gs, 0, nullptr);
//
//    // count the feature frames and return that
//    int frame_count = 0;
//    for (FeatureFrame * ff = gs.feature_frames[sliding_fork_hook.id]; ff->centre != SQUARE_SENTINEL; ++ff, ++frame_count);
//
//    return frame_count;
//}
//
//int queen_fork(const Gamestate & gs) {
//
//    find_queen_forks(gs, 0, nullptr);
//
//    // count the feature frames and return that
//    int frame_count = 0;
//    for (FeatureFrame * ff = gs.feature_frames[queen_fork_hook.id]; ff->centre != SQUARE_SENTINEL; ++ff, ++frame_count);
//
//    return frame_count;
//}
//
//void bench_forks() {
//
//    vector<Gamestate> states;
//    puzzle_gamestates(states);
//
//    bench("piece-forks", MICROS, states.data(), states.size(), &piece_fork);
//    bench("queen-forks", MICROS, states.data(), states.size(), &queen_fork);
//
//}

void bench_cands() {

    cout << "Benching candidate moves\n";

    vector<Gamestate> gamestates;
    puzzle_gamestates(gamestates);

    bench("cands", MICROS, gamestates.data(), gamestates.size(), &wrap_cands);
}
