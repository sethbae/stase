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
#include "src/game/gamestate.hpp"
#include "src/bench/bench.h"
#include "src/game/cands/responder.hpp"
#include "src/test/test.h"
//#include "src/test/test.h"
//#include "src/test/game/cands/fork_helpers.h"


/**
 * Runs cands on the first hundred thousand puzzles. Prints out a histogram showing for each decile,
 * how many puzzles fell into that bin (for number of candidates generated).
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
            case CRITICAL: size = cands(states[i], new CandSet)->critical.size(); break;
            case MEDIAL: size = cands(states[i], new CandSet)->medial.size(); break;
            case FINAL: size = cands(states[i], new CandSet)->final.size(); break;
            case LEGAL: size = cands(states[i], new CandSet)->legal.size(); break;
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

inline int count_frames(const Gamestate & gs, int hook_id) {
    int sum = 0;
    for (int i = 0; i < MAX_FRAMES && !is_sentinel(gs.frames[hook_id][i].centre); ++i) {
        ++sum;
    }
    return sum;
}

void number_of_frames_hist(const Hook * h) {

    std::vector<Gamestate> states;
    puzzle_gamestates(states);

    cout << "loaded states\n";

    int N = 100000;

    double num_frames[N];

    int max_size = 0;
    for (int i = 0; i < N; ++i) {
        cands(states[i], new CandSet);
        int size = count_frames(states[i], h->id);
        max_size = std::max(size, max_size);
        num_frames[i] = (double)size;
    }

    cout << "For " << h->name << ":\n";

    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
        sum += num_frames[i];
    }

    int n = 10;

    int hist_bins[n];
    for (int i = 0; i < n; ++i) {
        hist_bins[i] = 0;
    }

    for (int i = 0; i < N; ++i) {
        num_frames[i] /= ((double)max_size);
    }

    for (int i = 0; i < n; ++i) {
        double lower_bound = i / ((double)n);
        double upper_bound = (i + 1) / ((double)n);
        for (int j = 0; j < N;  ++j) {
            if (lower_bound <= num_frames[j] && num_frames[j] <= upper_bound) {
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
            case CRITICAL: size = cands(states[i], new CandSet)->critical.size(); break;
            case MEDIAL: size = cands(states[i], new CandSet)->medial.size(); break;
            case FINAL: size = cands(states[i], new CandSet)->final.size(); break;
            case LEGAL: size = cands(states[i], new CandSet)->legal.size(); break;
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

void q_scores() {

    std::vector<Gamestate> states;
    puzzle_gamestates(states);

    cout << "loaded states\n";

    int N = 100000;

    int q_score[N];

    int max_size = 0;
    for (int i = 0; i < N; ++i) {

        CandSet cs = *cands(states[i], new CandSet);
        if (cs.size() == 1000) {
            exit(1);
        }

        q_score[i] = (int) quiess(states[i]);
        if (q_score[i] > max_size) {
            max_size = q_score[i];
        }
    }

    cout << "got q scores\n";

    int sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += q_score[i];
    }

    int n = max_size + 1;

    int hist_bins[n];
    for (int i = 0; i < n; ++i) {
        hist_bins[i] = 0;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < N;  ++j) {
            if (q_score[j] == i) {
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

        if (!has_safe_king(states[i], states[i].board.get_white() ? WHITE : BLACK)) {
            // skip positions which are in check, they get handled differently
            continue;
        }

        int size;
        switch (cand_list) {
            case CRITICAL: size = cands(states[i], new CandSet)->critical.size(); break;
            case MEDIAL: size = cands(states[i], new CandSet)->medial.size(); break;
            case FINAL: size = cands(states[i], new CandSet)->final.size(); break;
            case LEGAL: size = cands(states[i], new CandSet)->legal.size(); break;
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
    for (int i = 0; !is_sentinel(gs.frames[h->id][i].centre) && i < MAX_FRAMES; ++i) {
        FeatureFrame ff = gs.frames[h->id][i];
        cout << "FeatureFrame: " << sqtos(ff.centre) << " " << sqtos(ff.secondary);
        cout << " c1: " << ff.conf_1 << " c2: " << ff.conf_2 << "\n";
    }

}

void show_hook_frames(const std::string & fen, const Hook * h) {
    Gamestate gs(fen);
    show_hook_frames(gs, h);
}

void run_engine(const std::string & fen, const double seconds) {
    run_in_background(fen);
    sleep(seconds);
    stop_engine(false);
    Move m = fetch_best_move();
    cout << "Engine played: " << mtos(fen_to_board(fen), m) << "\n";
}

SearchNode * repl_cycles(const std::string & fen) {

    std::string input;

    cout << "Enter number of cycles to analyse for: ";
    std::cin >> input;
    int cycles = std::stoi(input);

    cout << "\nAnalysing...";
    cout.flush();

    Gamestate * root_gs = new Gamestate(fen);
    SearchNode * root = new SearchNode{
            root_gs,
            cands(*root_gs, new CandSet),
            heur(*root_gs),
            MOVE_SENTINEL,
            {},
            nullptr,
            nullptr,
            0
    };

    greedy_search(root, cycles);
    cout << "done\n";

    return root;

}

SearchNode * repl_seconds(const std::string & fen) {

    std::string input;

    cout << "Enter number of seconds to analyse for: ";
    std::cin >> input;
    int secs = std::stoi(input);

    cout << "\nAnalysing...";
    cout.flush();

    run_in_background(fen);
    sleep(secs);
    stop_engine(false);

    cout << "done\n";

    return fetch_root();

}

void repl(const std::string & fen) {

    std::string input;

    cout << "Run engine for cycles or for seconds (c/s)? ";
    std::cin >> input;
    bool use_seconds = (input == "s");

    SearchNode * root;

    if (use_seconds) {
        root = repl_seconds(fen);
    } else {
        root = repl_cycles(fen);
    }

    bool cont = true;
    SearchNode * current = root;
    std::vector<SearchNode *> current_line{current};

    while (cont) {

        // print out a summary of the current position
        write_to_file(current, cout);
        cout << "\nCurrent line is: ";
        for (int i = 1; i < current_line.size(); ++i) {
            cout << mtos(current_line[i-1]->gs->board, current_line[i]->move) << " ";
        }
        if (current_line.size() == 1) {
            cout << "-";
        }
        cout << "\n";
        cout << "Total nodes in tree: " << subtree_size(root) << "\n";

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
            } else if (input == "quit" || input == "exit") {
                cont = false;
                read_input = true;
            } else if (input == "cands" || input == "c") {
                cands_report(*current->gs);
                read_input = true;
            } else if (input == "heur" || input == "h") {
                heur_with_description(*current->gs);
                read_input = true;
            } else if (input == "quiess" || input == "q") {
                cout << "Quiess: " << quiess(*current->gs) << "\n";
                read_input = true;
            } else {

                bool found = false;
                for (int i = 0; i < current->children.size(); ++i) {
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

//   // original benchmark position
//    const std::string fen =
//            std::string(
//                "r1bqk2r/1p2bppp/p1nppn2/2p5/2B1P3/2NP1Q2/PPP1NPPP/R1B2RK1 w kq - 0 8"
//            );

    const std::string fen = "1rr3k1/R4R2/3q2pQ/2pP4/2Pb4/1P3B2/5PPP/6K1 b - - 0 27";

    Gamestate gs(fen, MIDGAME);
    pr_board(gs.board);

//    for (const Move m : legal_moves(gs.board)) {
//        cout << mtos(gs.board, m) << " ";
//    }
//    cout << "\n";

//    CandSet * c = cands(gs);
//    print_cand_set(gs, *c, cout);

//    q_scores();

//    repl(fen);

    run_with_node_limit(fen, 25000);
    std::cout << fetch_node_count() << "\n";

//    greedy_search(fen, 15);

//    show_hook_frames(gs, &fork_hook);

//    show_responder_moves(fen, defend_centre_resp, FeatureFrame{stosq("c4"), {0, 0}, 0, 0});

//    cands_report(gs);

//    Eval score = heur_with_description(gs);

//    std::vector<Move> best_line = greedy_search(fen, 50000);

//    number_of_cands(CRITICAL);

    return 0;

}
