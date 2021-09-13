#include "board.h"
#include "game.h"
#include "cands.h"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;

const int NUM_FEATURES = 10;

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

    Move all_moves[MAX_TOTAL_CANDS];
    int m = 0;

    int i = 0;
    FeatureHandler fh;
    while ((fh = feature_handlers[i]).hook != nullptr) {
        MoveSet moves;
        int move_counter = 0;

        // run the predicate over the board
        (*fh.hook)(gs.board, &gs.feature_frames[i]);

        // for each feature frame, run either enemy or friendly responders over it
        for (int j = 0; gs.feature_frames[i][j].centre != SQUARE_SENTINEL; ++j) {
            FeatureFrame ff = gs.feature_frames[i][j];
            bool centre_piece_is_white = (colour(gs.board.get(ff.centre)) == WHITE);

            Responder ** responders =
                    (gs.board.get_white() == centre_piece_is_white)
                    ? fh.friendly_responses
                    : fh.enemy_responses;

            for (int k = 0; responders[k] != nullptr; ++k) {
                (*responders[k])(gs.board, &ff, &moves, move_counter);
            }

            if (move_counter == MAX_MOVES_PER_HOOK || m + move_counter >= MAX_TOTAL_CANDS) {
                break;
            }
        }

        if (move_counter < MAX_MOVES_PER_HOOK) {
            moves.moves[move_counter] = MOVE_SENTINEL;
        }

        // add moves not yet present
        for (int j = 0; j < MAX_MOVES_PER_HOOK && !is_sentinel(moves.moves[j]); ++j) {
            bool present = false;
            for (int k = 0; k < m; ++k) {
                if ((all_moves[k].from == moves.moves[j].from)
                        && (all_moves[k].to == moves.moves[j].to)) {
                    present = true;
                    break;
                }
            }
            if (!present && m < MAX_TOTAL_CANDS) {
                all_moves[m++] = moves.moves[j];
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
//
//void weak_hook(const Board & b, FeatureFrame** frame) {
//
//    Square hits[64];
//    int min_ws[64];
//    int min_bs[64];
//
//    int i = 0;
//
//    for (int x = 0; x < 8; ++x) {
//        for (int y = 0; y < 8; ++y) {
//            if (capture_walk(b, mksq(x, y), min_ws[i], min_bs[i])) {
//                hits[i++] = mksq(x, y);
//            }
//        }
//    }
//
//    *frame = static_cast<FeatureFrame*> (operator new((sizeof(FeatureFrame)) * (i + 1)));
//
//    for (int j = 0; j < i; ++j) {
//        (*frame)[j] = FeatureFrame{hits[j], 0, min_ws[j], min_bs[j]};
//    }
//    (*frame)[i] = FeatureFrame{SQUARE_SENTINEL, SQUARE_SENTINEL, 0, 0};
//
//}
//
//void weak_resp(const Board & b, MoveSet* moves, FeatureFrame* frame) {
//    // check frame and turn, delegate to major and minor adding moves.
//    bool white_to_play = b.get_white();
//    int move_count = 0;
//
//    for (FeatureFrame* ff = frame; ff->centre != SQUARE_SENTINEL; ff++) {
//        bool weak_piece_is_white = (colour(b.get(ff->centre)) == WHITE);
//        if (white_to_play == weak_piece_is_white) {
//            defend_piece(b, ff->centre, moves, move_count, ff->conf_1, ff->conf_2);
//        } else {
//            capture_piece(b, ff->centre, moves, move_count, ff->conf_1, ff->conf_2);
//        }
//        if (move_count == MAX_MOVES_PER_HOOK) {
//            return;
//        }
//    }
//
//    if (move_count < MAX_MOVES_PER_HOOK) {
//        moves->moves[move_count] = MOVE_SENTINEL;
//    }
//}
