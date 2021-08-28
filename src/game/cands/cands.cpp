#include "board.h"
#include "game.h"
#include "cands.h"

#include <vector>

using std::vector;

FeatureHandler feature_handlers[] = {
        {&weak_hook, &weak_resp},
        nullptr
};

FeatureHandler FEATURE_SENTINEL = {nullptr, nullptr};

const int MAX_MOVES = 10;

// return candidate moves for a board; as a minimal working example,
// return all legal moves
//vector<Move> cands(const Gamestate & gs) {
//
//    vector<Move> v;
//    legal_moves(gs.board, v);
//    return v;
//
//}

vector<Move> cands(const Gamestate &gs) {

    Move all_moves[MAX_MOVES];
    int m = 0;

    int i = 0;
    FeatureHandler fh;
    while ((fh = feature_handlers[i]).hook) {
        MoveSet moves;
        (*fh.hook)(gs.board, gs.features[i]);
        (*fh.responder)(gs.board, &moves, gs.features[i]);

        for (int j = 0; j < MAX_MOVES_PER_HOOK(); ++j) {
            bool present = false;
            for (int k = 0; k < m; ++k) {
                if ((all_moves[k].from == moves[j].from)
                        && (all_moves[k].to == moves[j].to)) {
                    present = true;
                }
            }
            if (!present && m < MAX_MOVES - 1) {
                all_moves[m++] = moves[j];
            }
        }

        ++i;
    }

    vector<Move> vec;
    for (int j = 0; j < m; ++j) {
        vec.push_back(all_moves[j]);
    }

    return vec;

}
