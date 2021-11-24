#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <fstream>
using std::ofstream;

#include "include/stase/board.h"
#include "include/stase/game.h"
#include "include/stase/search.h"
#include "include/stase/puzzle.h"
#include "src/game/cands/cands.h"
#include "src/bench/bench.h"
#include "src/test/test.h"
#include <unistd.h>

void number_of_cands() {

    std::vector<Gamestate> states;
    puzzle_gamestates(states);

    cout << "loaded states\n";

    int N = 100000;

    double num_cands[N];

    int max_size = 0;
    for (int i = 0; i < N; ++i) {
        int size = cands(states[i]).size();
        max_size = std::max(size, max_size);
        num_cands[i] = (double)size;
    }

    cout << "found cands\n";

    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
        sum += num_cands[i];
    }

    int n = 10;

    int hist_bins[n];
    for (int i = 0; i < n; ++i) {
        hist_bins[i] = 0;
    }

    for (int i = 0; i < N; ++i) {
        num_cands[i] /= ((double)max_size);
    }

    for (int i = 0; i < n; ++i) {
        double lower_bound = i / ((double)n);
        double upper_bound = (i + 1) / ((double)n);
        for (int j = 0; j < N;  ++j) {
            if (lower_bound <= num_cands[j] && num_cands[j] <= upper_bound) {
                hist_bins[i]++;
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        cout << "    " << i;
    }
    cout << "\n";

    for (int i = 0; i < n; ++i) {
        cout << hist_bins[i] << "  ";
    }
    cout << "\n";

    cout << "MAX: " << max_size << "\n";
    cout << "MEAN: " << sum / ((double)N) << "\n";

}

/**
 * Runs the responder on the given board, using a feature frame centred on the
 * given square
 */
void show_responder_moves(const std::string & fen, const Responder & resp, const FeatureFrame ff) {

    Gamestate gs(fen_to_board(fen));

    pr_board(gs.board);

    Move moves[100];
    IndexCounter counter(100);

    resp.resp(gs, &ff, moves, counter);

    cout << "Responder moves (" << resp.name << "):\n";
    for (int i = 0; i < counter.idx(); ++i) {
        cout << mtos(gs.board, moves[i]) << "\n";
    }

}

/**
 * Shows the feature frames discovered by the given hook on the gamestate
 */
void show_hook_frames(const std::string & fen, const Hook * h) {

    Gamestate gs(fen_to_board(fen));

    discover_feature_frames(gs, h);

    pr_board(gs.board);
    cout << "\nFeatureFrames found for " << h->name << ":\n";
    for (FeatureFrame * ff = gs.feature_frames[h->id]; !is_sentinel(ff->centre); ++ff) {
        cout << "FeatureFrame: " << sqtos(ff->centre) << " " << sqtos(ff->secondary);
        cout << " c1: " << ff->conf_1 << " c2: " << ff->conf_2 << "\n";
    }

}

int main(int argc, char** argv) {

    const std::string fen = std::string("Q7/8/8/K7/8/6k1/8/3q4 w - - 0 1");
    const FeatureFrame ff{stosq("a5"), stosq("a8"), 0, -1};

    show_responder_moves(fen, pin_skewer_resp, ff);

    Gamestate gs(fen_to_board(fen));

//    pr_board_conf(gs.board);
//
//    iterative_deepening_search(fen, 10);

    return 0;

}
