#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <cstring>

using std::ofstream;

#include "include/stase/board.h"
#include "include/stase/game.h"
#include "include/stase/search.h"
#include "include/stase/puzzle.h"
#include "src/game/cands/cands.h"
#include "src/bench/bench.h"
#include "src/test/test.h"



/**
 * Runs cands on the first hundred thousand puzzles. Prints out a histogram showing for each decile,
 * how many pyzzles fell into that bin (for number of candidates generated).
 */
void number_of_cands_hist(CandList cand_list) {

    std::vector<Gamestate> states;
    puzzle_gamestates(states);

    cout << "loaded states\n";

    int N = 100000;

    double num_cands[N];

    int max_size = 0;
    for (int i = 0; i < N; ++i) {
        int size;
        switch (cand_list) {
            case CRITICAL: size = cands(states[i])->critical.size(); break;
            case MEDIAL: size = cands(states[i])->medial.size(); break;
            case FINAL: size = cands(states[i])->final.size(); break;
            case LEGAL: size = cands(states[i])->legal.size(); break;
        }
        max_size = std::max(size, max_size);
        num_cands[i] = (double)size;
    }

    cout << "found cands\n";

    switch (cand_list) {
        case CRITICAL: cout << "CRITICAL\n"; break;
        case MEDIAL: cout << "MEDIAL\n"; break;
        case FINAL: cout << "FINAL\n"; break;
        case LEGAL: cout << "LEGAL\n"; break;
    }

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

void number_of_cands(CandList cand_list) {

    std::vector<Gamestate> states;
    puzzle_gamestates(states);

    cout << "loaded states\n";

    int N = 100000;

    double num_cands[N];

    int max_size = 0;
    for (int i = 0; i < N; ++i) {
        int size;
        switch (cand_list) {
            case CRITICAL: size = cands(states[i])->critical.size(); break;
            case MEDIAL: size = cands(states[i])->medial.size(); break;
            case FINAL: size = cands(states[i])->final.size(); break;
            case LEGAL: size = cands(states[i])->legal.size(); break;
        }
        max_size = max(size, max_size);
        num_cands[i] = (double)size;
    }

    cout << "found cands: " << name(cand_list) << "\n";

    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
        sum += num_cands[i];
    }

    int n = max_size + 1;

    int hist_bins[n];
    for (int i = 0; i < n; ++i) {
        hist_bins[i] = 0;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < N;  ++j) {
            if (num_cands[j] == i) {
                hist_bins[i]++;
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        cout << "     " << i;
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
 * Runs cands on the first 100k puzzles, and then prints out the FENs of puzzles which generate either the
 * maximum number of candidates, or the maximum minus one.
 */
void find_cands_outliers(CandList cand_list) {

    std::vector<Gamestate> states;
    puzzle_gamestates(states);

    cout << "loaded states\n";

    int N = 100000;

    double num_cands[N];

    int max_size = 0;
    for (int i = 0; i < N; ++i) {

        if (!is_safe_king(states[i], states[i].board.get_white() ? WHITE : BLACK)) {
            // skip positions which are in check, they get handled differently
            continue;
        }

        int size;
        switch (cand_list) {
            case CRITICAL: size = cands(states[i])->critical.size(); break;
            case MEDIAL: size = cands(states[i])->medial.size(); break;
            case FINAL: size = cands(states[i])->final.size(); break;
            case LEGAL: size = cands(states[i])->legal.size(); break;
        }

        max_size = max(size, max_size);
        num_cands[i] = (double)size;

    }

    cout << "found cands\n";

    switch (cand_list) {
        case CRITICAL: cout << "CRITICAL\n"; break;
        case MEDIAL: cout << "MEDIAL\n"; break;
        case FINAL: cout << "FINAL\n"; break;
        case LEGAL: cout << "LEGAL\n"; break;
    }

    cout << "Max was: " << max_size << "\n";

    for (int i = 0; i < N; ++i) {
        if (num_cands[i] >= max_size - 1) {
            cout << "(" << num_cands[i] << ") " << board_to_fen(states[i].board) << "\n";
        }
    }

}

/**
 * Runs the responder on the given board, using a feature frame centred on the
 * given square
 */
void show_responder_moves(const std::string & fen, const Responder & resp, const FeatureFrame ff) {

    Gamestate gs(fen);

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
void show_hook_frames(Gamestate & gs, const Hook * h) {

    discover_feature_frames(gs, h);

    pr_board(gs.board);
    cout << "\nFeatureFrames found for " << h->name << ":\n";
    for (FeatureFrame * ff = gs.feature_frames[h->id]; !is_sentinel(ff->centre); ++ff) {
        cout << "FeatureFrame: " << sqtos(ff->centre) << " " << sqtos(ff->secondary);
        cout << " c1: " << ff->conf_1 << " c2: " << ff->conf_2 << "\n";
    }

}

void show_hook_frames(const std::string & fen, const Hook * h) {
    Gamestate gs(fen);
    show_hook_frames(gs, h);
}

Move run_engine(const std::string & fen, const double seconds) {
    run_in_background(fen);
    sleep(seconds);
    stop_engine();
    return fetch_best_move();
}

void repl(const std::string & fen) {

    cout << "Enter number of cycles to analyse for: ";
    std::string input;
    std::cin >> input;
    int cycles = std::stoi(input);
    cout << "\nAnalysing...";

    Gamestate root_gs(fen);
    SearchNode root{
        &root_gs,
        cands(root_gs),
        heur(root_gs),
        MOVE_SENTINEL,
        0,
        nullptr,
        nullptr
    };

    greedy_search(&root, cycles);
    cout << "done\n";

    bool cont = true;
    SearchNode * current = &root;
    std::vector<SearchNode *> current_line{current};

    while (cont) {

        // print out a summary of the current position
        write_to_file(current, cout);
        cout << "\nCurrent line is: ";
        for (int i = 1; i < current_line.size(); ++i) {
            cout << mtos(current_line[i]->gs->board, current_line[i]->move) << " ";
        }
        if (current_line.size() == 1) {
            cout << "-";
        }
        cout << "\n";

        // get the next instruction
        bool read_input = false;
        while (!read_input) {

            cout << "\n>> ";
            std::cin >> input;

            if (input == "back" || input == "b") {
                if (current_line.size() <= 1) {
                    break;
                }
                current = current_line[current_line.size() - 2];
                current_line.pop_back();
                read_input = true;
            } else if (input == "quit" || input == "q") {
                cont = false;
                read_input = true;
            } else if (input == "cands" || input == "c") {
                cands_report(*current->gs);
                read_input = true;
            } else {

                bool found = false;
                for (int i = 0; i < current->num_children; ++i) {
                    Move m = current->children[i]->move;
                    if (mtos(current->gs->board, m) == input) {
                        found = true;
                        current_line.push_back(current->children[i]);
                        current = current->children[i];
                        break;
                    }
                }

                if (!found) {
                    cout << "\nCould not find move";
                } else {
                    read_input = true;
                }
            }
        }
    }
}

int main(int argc, char** argv) {

    signal(SIGSEGV, print_stack_trace);
    signal(SIGABRT, print_stack_trace);

    const std::string fen =
            std::string(
            "r7/1p3pkp/4b1p1/Pp6/1n6/3p1PP1/r7/R5KR w - - 2 28"
            );

//    Gamestate gs(fen);
//    pr_board(gs.board);

    repl(fen);

//    run_engine(fen, 10);

//    show_hook_frames(gs, &unsafe_piece_hook);

//    cands_report(gs);

//    heur_with_description(gs);

//    std::vector<Move> best_line = greedy_search(fen, 50000);

//    number_of_cands(CRITICAL);

    return 0;

}
