#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../../game/gamestate.hpp"
#include "../../test.h"

bool stress_test_individual_hook(std::vector<Gamestate> & states, const Hook * h) {

    cout << "Stress testing " << h->name << "\n";
    cout << "|                   |\n";

    for (int i = 0; i < states.size(); ++i) {

        if ((i % (states.size() / 20)) == 0) {
            cout << "*";
            cout.flush();
        }

        Gamestate & gs = states[i];

        discover_feature_frames(gs, h);

        FeatureFrame ff;
        for (int j = 0; !is_sentinel((ff = gs.feature_frames[h->id][j]).centre); ++j) {

            if (!val(ff.centre)) {
                cout << "\n[" << j << "] FAILED: Received " << sqtos(ff.centre) << "\n";
                cout << board_to_fen(gs.board) << "\n";
                pr_board(gs.board);
                return false;
            }
        }
    }

    cout << "\nAll tests passed\n";
    return true;
}

bool stress_test_cands(std::vector<Gamestate> & states) {

    cout << "Stress testing candidates\n";
    cout << "|                   |\n";

    for (int i = 0; i < states.size(); ++i) {

        if ((i % (states.size() / 20)) == 0) {
            cout << "*";
            cout.flush();
        }

        Gamestate & gs = states[i];

        std::vector<Move> moves = cands(gs)->critical;
        std::vector<Move> legals = legal_moves(gs.board);

        if (moves.size() > MAX_TOTAL_CANDS) {
            cout << "\n[" << i << "] FAILED: Received too many candidates (" << moves.size() << ")\n";
            cout << board_to_fen(gs.board) << "\n";
            pr_board(gs.board);
            return false;
        }

        for (int j = 0; j < moves.size(); ++j) {

            if (!val(moves[j].from) || !val(moves[j].to)) {
                cout << "\n[" << i << "] FAILED: Received " << sqtos(moves[j].from) << " to " << sqtos(moves[j].to) << "\n";
                cout << board_to_fen(gs.board) << "\n";
                pr_board(gs.board);
                return false;
            }

            bool is_legal = false;
            for (const Move & m : legals) {
                if (equal(m, moves[j])) {
                    is_legal = true;
                    break;
                }
            }

            if (!is_legal) {
                cout << "\n[" << i << "] FAILED: Received " << sqtos(moves[j].from) << " to " << sqtos(moves[j].to) << "\n";
                cout << board_to_fen(gs.board) << "\n";
                pr_board(gs.board);
                return false;
            }
        }
    }

    cout << "\nAll tests passed\n";
    return true;

}
