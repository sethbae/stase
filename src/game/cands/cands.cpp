#include "board.h"
#include "game.h"
#include "cands.h"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;

/**
 * Records the feature frames in the right place on the gamestate (allocating memory for them).
 * Also adds a sentinel to the end of the list.
 */
void record_hook_features(Gamestate & gs, const Hook * h, FeatureFrame * ff, int n) {

    gs.feature_frames[h->id] = static_cast<FeatureFrame*> (operator new((sizeof(FeatureFrame)) * (n + 1)));
    for (int i = 0; i < n; ++i) {
        gs.feature_frames[h->id][i] = ff[i];
    }
    gs.feature_frames[h->id][n] = FeatureFrame{SQUARE_SENTINEL, SQUARE_SENTINEL, 0, 0};

}

/**
 * Runs the given hook over every square on the board and records any successes in feature frames.
 */
void discover_feature_frames(Gamestate & gs, const Hook * hook) {

    std::vector<FeatureFrame> frames;
    frames.reserve(64);

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            hook->hook(gs, mksq(x, y), frames);
        }
    }

    record_hook_features(gs, hook, frames.data(), frames.size());
}

/**
 * Computes and returns a set of candidate moves specifically for positions when the
 * side to move is in check.
 */
vector<Move> cands_in_check(const Gamestate & gs) {
    // currently, there is no special logic.
    return legal_moves(gs.board);
}

/**
 * Computes and returns a set of candidate moves for the given position. Returns at most
 * MAX_TOTAL_CANDS moves which are guaranteed to be unique. No guarantee is made as to
 * the minimum number of moves returned.
 */
vector<Move> cands(Gamestate & gs) {

    // if we're in check, handle the candidates differently
    if (!is_safe_king(gs, gs.board.get_white() ? WHITE : BLACK)) {
        std::vector<Move> cands = cands_in_check(gs);
        if (cands.empty()) {
            gs.has_been_mated = true;
        }
        return cands;
    }

    Move all_moves[MAX_TOTAL_CANDS];
    int m = 0;

    for (int i = 0; i < feature_handlers.size(); ++i) {

        FeatureHandler fh = feature_handlers[i];
        Move moves[MAX_MOVES_PER_HOOK];
        IndexCounter counter(MAX_MOVES_PER_HOOK);

        // run the predicate over the board
        discover_feature_frames(gs, fh.hook);

        // for each feature frame, run either enemy or friendly responders over it
        for (int j = 0; !is_sentinel(gs.feature_frames[fh.hook->id][j].centre); ++j) {

            FeatureFrame ff = gs.feature_frames[fh.hook->id][j];
            bool centre_piece_is_white = (colour(gs.board.get(ff.centre)) == WHITE);

            std::vector<const Responder *> responders =
                    (gs.board.get_white() == centre_piece_is_white)
                    ? fh.friendly_responses
                    : fh.enemy_responses;

            for (int k = 0; k < responders.size() && counter.has_space(); ++k) {
                responders[k]->resp(gs, &ff, moves, counter);
            }

            if (!counter.has_space()) {
                break;
            }
        }

        // add moves not yet present
        for (int j = 0; j < counter.idx(); ++j) {
            bool present = false;
            for (int k = 0; k < m; ++k) {
                if (equal(all_moves[k], moves[j])) {
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

    vector<Move> vec;
    for (int j = 0; j < m; ++j) {
        vec.push_back(all_moves[j]);
    }

    return vec;

}

/**
 * Generates candidate moves for the gamestate, while printing information to stdout.
 * Useful for debugging (keep up to date with real implementation).
 */
vector<Move> cands_report(Gamestate & gs) {

    cout << "********************************\n"
            "* Generating candidate moves\n"
            "********************************\n\n";

    pr_board_conf(gs.board);

    // if we're in check, handle the candidates differently
    if (!is_safe_king(gs, gs.board.get_white() ? WHITE : BLACK)) {

        std::vector<Move> cands = cands_in_check(gs);

        if (cands.empty()) {
            gs.has_been_mated = true;
            cout << "The position is checkmate - no candidates\n";
        } else{
            cout << "In check. Returning legal moves:\n";
            for (const Move & m : cands) {
                cout << mtos(gs.board, m);
            }
            cout << "\n";
        }

        return cands;
    }

    Move all_moves[MAX_TOTAL_CANDS];
    int m = 0;

    for (int i = 0; i < feature_handlers.size(); ++i) {

        FeatureHandler fh = feature_handlers[i];
        Move moves[MAX_MOVES_PER_HOOK];
        IndexCounter counter(MAX_MOVES_PER_HOOK);

        std::string friends = "";
        std::string enems = "";
        for (const Responder * r : fh.friendly_responses) { friends = friends + " " + r->name; }
        for (const Responder * r : fh.enemy_responses) { enems = enems + " " + r->name; }
        cout << "\n***** Feature handler " << i << "\n";
        cout << "* Hook: " << fh.hook->name << "\n";
        cout << "* Friendly:" << friends << "\n";
        cout << "* Enemy:" << enems << "\n";

        // run the predicate over the board
        discover_feature_frames(gs, fh.hook);

        cout << "\nFound frames:\n";
        for (int j = 0; !is_sentinel(gs.feature_frames[fh.hook->id][j].centre); ++j) {
            FeatureFrame ff = gs.feature_frames[fh.hook->id][j];
            cout << "Centre: " << sqtos(ff.centre) << " Second: " << sqtos(ff.secondary) << " c1: " << ff.conf_1 << " c2: " << ff.conf_2 << "\n";
        }
        cout << "\n\n";

        // for each feature frame, run either enemy or friendly responders over it
        for (int j = 0; !is_sentinel(gs.feature_frames[fh.hook->id][j].centre); ++j) {

            FeatureFrame ff = gs.feature_frames[fh.hook->id][j];
            bool centre_piece_is_white = (colour(gs.board.get(ff.centre)) == WHITE);

            cout << "Looking at frame " << j << " (" << sqtos(ff.centre) << ", " << sqtos(ff.secondary) << "):\n";

            std::vector<const Responder *> responders =
                    (gs.board.get_white() == centre_piece_is_white)
                    ? fh.friendly_responses
                    : fh.enemy_responses;

            for (int k = 0; k < responders.size() && counter.has_space(); ++k) {
                cout << "   Applying " << responders[k]->name << "\n";
                int prev_num = counter.idx();

                responders[k]->resp(gs, &ff, moves, counter);

                if (counter.idx() > prev_num) {
                    for (int l = prev_num; l < counter.idx(); ++l) {
                        cout << "      " << mtos(gs.board, moves[l]) << "\n";
                    }
                } else {
                    cout << "      No moves\n";
                }
            }

            if (!counter.has_space()) {
                break;
            }
        }

        // add moves not yet present
        for (int j = 0; j < counter.idx(); ++j) {
            bool present = false;
            for (int k = 0; k < m; ++k) {
                if (equal(all_moves[k], moves[j])) {
                    present = true;
                    break;
                }
            }
            if (!present && m < MAX_TOTAL_CANDS) {
                cout << "   Confirming " << mtos(gs.board, moves[j]) << "\n";
                all_moves[m++] = moves[j];
            } else if (m >= MAX_TOTAL_CANDS) {
                break;
            }
        }

    }

    pr_board(gs.board);
    cout << "Candidates generated:\n";
    vector<Move> vec;
    for (int j = 0; j < m; ++j) {
        cout << "Move from " << sqtos(all_moves[j].from) << " to " << sqtos(all_moves[j].to) << " (" << mtos(gs.board, all_moves[j]) << ")\n";
        vec.push_back(all_moves[j]);
    }

    return vec;

}
