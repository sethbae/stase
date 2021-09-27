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
#include "../../bench.h"

struct BenchHookParam {
    const Hook * h;
    const Board b;
};

bool run_hook(const BenchHookParam & b_h_p) {
    Gamestate gs(b_h_p.b);
    discover_feature_frames(gs, b_h_p.h);

    // count the feature frames and return that
    int frame_count = 0;
    for (FeatureFrame * ff = gs.feature_frames[0]; ff->centre != SQUARE_SENTINEL; ++ff, ++frame_count);

    return frame_count;
}

void bench_hook(const Hook * h) {

    vector<Board> boards;
    puzzle_boards(boards);

    vector<BenchHookParam> params;
    for (Board b : boards) {
        params.push_back(BenchHookParam{h, b});
    }

    bench(h->name, MICROS, params.data(), params.size(), &run_hook);

}

void bench_individual_hooks() {

    cout << "Benching individual hooks\n";

    for (int i = 0; i < ALL_HOOKS.size(); ++i) {
        bench_hook(&ALL_HOOKS[i]);
    }
}

int wrap_cands(const Gamestate & gs) {
    return cands(gs).size();
}

void bench_cands() {

    cout << "Benching candidate moves\n";

    vector<Gamestate> gamestates;
    puzzle_gamestates(gamestates);

    bench("cands", MICROS, gamestates.data(), gamestates.size(), &wrap_cands);
}
