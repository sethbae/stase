#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../test.h"

bool stress_test_individual_hook(std::vector<Gamestate> & states, const Hook * h) {

    cout << "|                  |\n";

    for (int i = 0; i < states.size(); ++i) {

        if ((i % (states.size() / 20)) == 0) {
            cout << "*";
            cout.flush();
        }

        Gamestate gs = states[i];

        discover_feature_frames(gs, h);

        FeatureFrame ff;
        for (int j = 0; (ff = gs.feature_frames[h->id][j]).centre != SQUARE_SENTINEL; ++j) {

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

    cout << "|                  |\n";

    for (int i = 0; i < states.size(); ++i) {

        if ((i % (states.size() / 20)) == 0) {
            cout << "*";
            cout.flush();
        }

        Gamestate gs = states[i];

        std::vector<Move> moves = cands(gs);

        for (int j = 0; j < moves.size(); ++j) {

            if (!val(moves[j].from) || !val(moves[j].to)) {
                cout << "\n[" << j << "] FAILED: Received " << sqtos(moves[j].from) << " to " << moves[j].to << "\n";
                cout << board_to_fen(gs.board) << "\n";
                pr_board(gs.board);
                return false;
            }
        }
    }

    cout << "\nAll tests passed\n";
    return true;

}
