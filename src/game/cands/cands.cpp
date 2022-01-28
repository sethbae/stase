#include "board.h"
#include "game.h"
#include "cands.h"
#include "hook.hpp"
#include "responder.hpp"
#include "features.hpp"
#include "../gamestate.hpp"

#include <vector>
using std::vector;
#include <iostream>
using std::cout;
#include <iomanip>

/**
 * Runs the given hook over every square on the board and records any successes in feature frames.
 */
void discover_feature_frames(Gamestate & gs, const Hook * hook) {

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            bool result = hook->hook(gs, mksq(x, y));
            if (!result) { return; }
        }
    }

}

/**
 * Computes and returns a set of candidate moves specifically for positions when the
 * side to move is in check.
 */
CandSet * cands_in_check(const Gamestate & gs, CandSet * cand_set) {
    // currently, there is no special logic.
    cand_set->critical = legal_moves(gs.board);
    return cand_set;
}

CandSet * cands(Gamestate & gs) {
    return cands(gs, new CandSet);
}

/**
 * Computes and returns a set of candidate moves for the given position. Returns at most
 * MAX_TOTAL_CANDS moves which are guaranteed to be unique. No guarantee is made as to
 * the minimum number of moves returned.
 */
CandSet * cands(Gamestate & gs, CandSet * cand_set) {

    // if we're in check, handle the candidates differently
    if (!is_safe_king(gs, gs.board.get_white() ? WHITE : BLACK)) {
        cands_in_check(gs, cand_set);
        gs.in_check = true;
        if (cand_set->empty()) {
            gs.has_been_mated = true;
        }
        return cand_set;
    }

    Move all_moves[MAX_TOTAL_CANDS];
    int m = 0;

    for (int i = 0; i < feature_handlers.size() && m < MAX_TOTAL_CANDS; ++i) {

        FeatureHandler fh = feature_handlers[i];
        Move moves[MAX_MOVES_PER_HOOK];
        IndexCounter counter(MAX_MOVES_PER_HOOK);

        // run the predicate over the board
        discover_feature_frames(gs, fh.hook);

        // for each feature frame, run either enemy or friendly responders over it
        for (int j = 0; !is_sentinel(gs.frames[fh.hook->id][j].centre) && j < MAX_FRAMES; ++j) {

            FeatureFrame ff = gs.frames[fh.hook->id][j];
            bool centre_piece_is_white = (colour(gs.board.get(ff.centre)) == WHITE);

            std::vector<const Responder *> responders =
                    (gs.board.get_white() == centre_piece_is_white)
                    ? fh.friendly_responses
                    : fh.enemy_responses;

            counter.clear_soft_limit();
            counter.add_allowance(MAX_MOVES_PER_FRAME);

            for (int k = 0; k < responders.size() && counter.has_space(); ++k) {
                responders[k]->resp(gs, &ff, moves, counter);
            }

            counter.clear_soft_limit();
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
                    // combine the scores: 1 + max
                    all_moves[k].set_score(max(1 + all_moves[k].get_score(), moves[j].get_score()));
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

    int best_score = 0;
    for (int i = 0; i < m; ++i) {
        if (all_moves[i].get_score() > best_score) {
            best_score = all_moves[i].get_score();
        }
    }

    for (int j = 0; j < m; ++j) {
        int score = all_moves[j].get_score();
        if (score > 1 && score >= best_score / 2) {
            cand_set->critical.push_back(all_moves[j]);
        } else if (score > 0) {
            cand_set->medial.push_back(all_moves[j]);
        } else {
            cand_set->final.push_back(all_moves[j]);
        }
    }

    if (m == 0) {
        cand_set->legal = legal_moves(gs.board);
    }

    return cand_set;

}

/**
 * Generates candidate moves for the gamestate, while printing information to stdout.
 * Useful for debugging (keep up to date with real implementation).
 */
CandSet * cands_report(Gamestate & gs) {

    cout << "********************************\n"
            "* Generating candidate moves\n"
            "********************************\n\n";

    pr_board_conf(gs.board);
    CandSet * cand_set = new CandSet;

    // if we're in check, handle the candidates differently
    if (!is_safe_king(gs, gs.board.get_white() ? WHITE : BLACK)) {

        cands_in_check(gs, cand_set);

        if (cand_set->empty()) {
            gs.has_been_mated = true;
            cout << "The position is checkmate - no candidates\n";
        } else{
            cout << "In check. Returning legal moves:\n";
            for (const Move & m : cand_set->critical) {
                cout << mtos(gs.board, m);
            }
            cout << "\n";
        }

        return cand_set;
    }

    Move all_moves[MAX_TOTAL_CANDS];
    int m = 0;

    for (int i = 0; i < feature_handlers.size() && m < MAX_TOTAL_CANDS; ++i) {

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
        for (int j = 0; !is_sentinel(gs.frames[fh.hook->id][j].centre); ++j) {
            FeatureFrame ff = gs.frames[fh.hook->id][j];
            cout << "Centre: " << sqtos(ff.centre) << " Second: " << sqtos(ff.secondary) << " c1: " << ff.conf_1 << " c2: " << ff.conf_2 << "\n";
        }
        cout << "\n\n";

        // for each feature frame, run either enemy or friendly responders over it
        for (int j = 0; !is_sentinel(gs.frames[fh.hook->id][j].centre); ++j) {

            FeatureFrame ff = gs.frames[fh.hook->id][j];
            bool centre_piece_is_white = (colour(gs.board.get(ff.centre)) == WHITE);

            cout << "Looking at frame " << j << " (" << sqtos(ff.centre) << ", " << sqtos(ff.secondary) << "):\n";

            std::vector<const Responder *> responders =
                    (gs.board.get_white() == centre_piece_is_white)
                    ? fh.friendly_responses
                    : fh.enemy_responses;

            counter.clear_soft_limit();
            counter.add_allowance(MAX_MOVES_PER_FRAME);

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

            counter.clear_soft_limit();
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
                    // combine the scores: 1 + max
                    all_moves[k].set_score(max(1 + all_moves[k].get_score(), moves[j].get_score()));
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

    int best_score = 0;
    for (int i = 0; i < m; ++i) {
        if (all_moves[i].get_score() > best_score) {
            best_score = all_moves[i].get_score();
        }
    }

    for (int j = 0; j < m; ++j) {
        int score = all_moves[j].get_score();
        if (score > 1 && score >= best_score / 2) {
            cand_set->critical.push_back(all_moves[j]);
        } else if (score > 0) {
            cand_set->medial.push_back(all_moves[j]);
        } else {
            cand_set->final.push_back(all_moves[j]);
        }
    }

    if (m == 0) {
        cand_set->legal = legal_moves(gs.board);
    }

    pr_board(gs.board);
    print_cand_set(gs, *cand_set, cout);

    return cand_set;

}

void print_cand_set(const Gamestate & gs, const CandSet & cand_set, std::ostream & o) {
    o << "Candidates generated:\n";
    o << std::setw(10) << "Critical: ";
    for (const Move & move : cand_set.critical) {
        o << std::setw(5) << mtos(gs.board, move) << "[" << move.get_score() << "] ";
    }
    o << "\n";

    o << std::setw(10) << "Medial: ";
    for (const Move & move : cand_set.medial) {
        o <<  std::setw(5) << mtos(gs.board, move) << "[" << move.get_score() << "] ";
    }
    o << "\n";

    o << std::setw(10) << "Final: ";
    for (const Move & move : cand_set.final) {
        o  << std::setw(5) << mtos(gs.board, move) << "[" << move.get_score() << "] ";
    }
    o << "\n";

    o << std::setw(10) << "Legal: ";
    for (const Move & move : cand_set.legal) {
        o << std::setw(5) << mtos(gs.board, move);
    }
    o << "\n";
}
