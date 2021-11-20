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


//
//void record_tree_in_file(const std::string & filename, const SearchNode * nodes) {
//
//    ofstream file;
//    file.open(filename, std::ios::out);
//
//    file << "This file contains engine analysis of the following position\n";
//
//    readable_printout(nodes, file);
//
//    file << "NB score is inherited from best successors, computed only for leaves\n";
//    file << "\nHere are the nodes:\n";
//
//    write_to_file(nodes[nodes.size() - 1], file);
//    file.close();
//
//}

//void analyse_position(const std::string & fen_str) {
//    Gamestate gs;
//    gs.board = fen_to_board(fen_str);
//
//    pr_board(gs.board);
//
//    vector<SearchNode*> nodes;
//
//    nodes = depth_limited_search(gs, 5);
//    //write_to_file("test", nodes);
//
//    readable_printout(nodes, cout);
//}


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
void show_responder_moves(const std::string & fen, const Responder & resp, const Square sq) {

    Gamestate gs(fen_to_board(fen));

    Move moves[100];
    IndexCounter counter(100);

    develop_resp.resp(gs, new FeatureFrame{sq}, moves, counter);

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
        cout << "FeatureFrame: " << sqtos(ff->centre) << " " << sqtos(ff->secondary) << "\n";
    }

}

int main(int argc, char** argv) {

    Gamestate gs(fen_to_board("2q2r1k/8/5Pp1/2p1p1Pp/N1QbP3/3R2K1/P1P5/3R4 w - h6 0 37"));

    pr_board_conf(gs.board);

    return 0;

}
