#include "board.h"
#include "game.h"
#include "cands.h"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;

const int NUM_FEATURES = 10;

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
        (*fh.hook)(gs.board, &gs.feature_frames[i]);
        (*fh.responder)(gs.board, &moves, gs.feature_frames[i]);

        for (int j = 0; j < MAX_MOVES_PER_HOOK; ++j) {
            bool present = false;
            for (int k = 0; k < m; ++k) {
                if ((all_moves[k].from == moves.moves[j].from)
                        && (all_moves[k].to == moves.moves[j].to)) {
                    present = true;
                    break;
                }
            }
            if (!present && m < MAX_MOVES - 1) {
                all_moves[m++] = moves.moves[j];
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
