#include "board.h"
#include "game.h"
#include "cands.h"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;

const int NUM_FEATURES = 10;

/**
 * Runs the given hook over every square on the board and records any successes in feature frames.
 * The frames are written in an array, a pointer to which is put at the given address.
 *
 * @param b the board.
 * @param hook the predicate.
 * @param frames_ptr where to put the pointer to the results.
 */
void discover_feature_frames(const Board & b, Hook * hook, FeatureFrame ** frames_ptr) {

    Square hits[64];
    Square secondaries[64];
    int min_ws[64];
    int min_bs[64];

    int i = 0;

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if ((*hook)(b, mksq(x, y), &secondaries[i], &min_ws[i], &min_bs[i])) {
                hits[i++] = mksq(x, y);
            }
        }
    }

    *frames_ptr = static_cast<FeatureFrame*> (operator new((sizeof(FeatureFrame)) * (i + 1)));

    for (int j = 0; j < i; ++j) {
        (*frames_ptr)[j] = FeatureFrame{hits[j], secondaries[i], min_ws[j], min_bs[j]};
    }
    (*frames_ptr)[i] = FeatureFrame{SQUARE_SENTINEL, SQUARE_SENTINEL, 0, 0};

}

vector<Move> cands(const Gamestate &gs) {

    MoveCounter counter(MAX_TOTAL_CANDS);

    Move all_moves[MAX_TOTAL_CANDS];
    int m = 0;

    int i = 0;
    FeatureHandler fh;
    while ((fh = feature_handlers[i]).hook != nullptr) {
        Move moves[MAX_MOVES_PER_HOOK];
        counter.add_allowance(MAX_MOVES_PER_HOOK);

        // run the predicate over the board
        discover_feature_frames(gs.board, fh.hook, &gs.feature_frames[i]);

        // for each feature frame, run either enemy or friendly responders over it
        for (int j = 0; gs.feature_frames[i][j].centre != SQUARE_SENTINEL; ++j) {
            FeatureFrame ff = gs.feature_frames[i][j];
            bool centre_piece_is_white = (colour(gs.board.get(ff.centre)) == WHITE);

            Responder ** responders =
                    (gs.board.get_white() == centre_piece_is_white)
                    ? fh.friendly_responses
                    : fh.enemy_responses;

            for (int k = 0; responders[k] != nullptr && counter.has_space(); ++k) {
                (*responders[k])(gs.board, &ff, &moves[0], counter);
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

        ++i;
    }

    pr_board(gs.board);
    cout << "Candidates generated:\n";
    vector<Move> vec;
    for (int j = 0; j < m; ++j) {
        cout << "Move from " << sqtos(all_moves[j].from) << " to " << sqtos(all_moves[j].to) << "\n";
        vec.push_back(all_moves[j]);
    }

    return vec;

}
