#include "board.h"
#include "game.h"
#include "cands.h"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;

/**
 * Runs the given hook over every square on the board and records any successes in feature frames.
 *
 * @param gs the gamestate.
 * @param hook the predicate.
 * @param idx the index of the hook
 */
void discover_feature_frames(const Gamestate & gs, const Hook * hook) {

    FeatureFrame frames[64];

    int i = 0;

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (hook->hook(gs.board, mksq(x, y), &frames[i])) {
                frames[i++].centre = mksq(x, y);
            }
        }
    }

    gs.feature_frames[hook->id] = static_cast<FeatureFrame*> (operator new((sizeof(FeatureFrame)) * (i + 1)));

    for (int j = 0; j < i; ++j) {
        gs.feature_frames[hook->id][j] = frames[j];
    }
    gs.feature_frames[hook->id][i] = FeatureFrame{SQUARE_SENTINEL, SQUARE_SENTINEL, 0, 0};

}

vector<Move> cands(const Gamestate & gs) {

    IndexCounter counter(MAX_TOTAL_CANDS);

    Move all_moves[MAX_TOTAL_CANDS];
    int m = 0;

    for (int i = 0; i < feature_handlers.size(); ++i) {

        FeatureHandler fh = feature_handlers[i];
        Move moves[MAX_MOVES_PER_HOOK];
        counter.add_allowance(MAX_MOVES_PER_HOOK);

        // run the predicate over the board
        discover_feature_frames(gs, fh.hook);

        // for each feature frame, run either enemy or friendly responders over it
        for (int j = 0; gs.feature_frames[i][j].centre != SQUARE_SENTINEL; ++j) {

            FeatureFrame ff = gs.feature_frames[i][j];
            bool centre_piece_is_white = (colour(gs.board.get(ff.centre)) == WHITE);

            std::vector<const Responder *> responders =
                    (gs.board.get_white() == centre_piece_is_white)
                    ? fh.friendly_responses
                    : fh.enemy_responses;

            for (int k = 0; k < responders.size() && counter.has_space(); ++k) {
                responders[k]->resp(gs.board, &ff, moves, counter);
            }

            if (!counter.has_space()) {
                break;
            }
        }

        // add moves not yet present
        for (int j = 0; j < counter.idx(); ++j) {
            bool present = false;
            for (int k = 0; k < m; ++k) {
                if ((all_moves[k].from == moves[j].from)
                        && (all_moves[k].to == moves[j].to)) {
                    present = true;
                    break;
                }
            }
            if (!present && m < MAX_TOTAL_CANDS) {
                all_moves[m++] = moves[j];
            } else if (m >= MAX_TOTAL_CANDS) {
                break;
            }
        }

    }

//    pr_board(gs.board);
//    cout << "Candidates generated:\n";
    vector<Move> vec;
    for (int j = 0; j < m; ++j) {
        // cout << "Move from " << sqtos(all_moves[j].from) << " to " << sqtos(all_moves[j].to) << "\n";
        vec.push_back(all_moves[j]);
    }

    return vec;

}
