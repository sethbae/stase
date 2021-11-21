#include <iostream>
using std::cout;
using std::cin;

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <unistd.h>

#include <execinfo.h>
#include <signal.h>

#include "board.h"
#include "search.h"

void print_stack_trace(int sig) {

    void *array[25];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 25);

    // print out all the frames
    fprintf(stdout, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDOUT_FILENO);

    exit(1);
}

const std::string welcome_message =
        "Welcome to Stase v4.0\n";

void manual_gameplay() {

    Board b = starting_pos();
    vector<Move> moves;

    while (true) {

        pr_board_conf(b); cout << "\n";

        moves.clear();
        legal_moves(b, moves);

        for (Move m : moves) {
            cout << mtos(b, m) << " ";
        }
        cout << "\n\n";

        Move player_move = empty_move();
        bool valid_move = false;

        while (!valid_move) {

            cout << ">> ";

            string userIn;
            cin >> userIn;

            for (Move m : moves) {
                if (mtos(b, m) == userIn) {
                    player_move = m;
                    valid_move = true;
                    break;
                }
            }

            if (!valid_move) {
                cout << "Invalid\n";
            }
        }

        b = b.successor(player_move);

    }

}

void play_game(bool engine_is_white, int seconds_per_move) {

    Board b = starting_pos();
    bool players_turn = !engine_is_white;
    bool keep_playing = true;

    while (keep_playing) {

        pr_board(b);
        std::vector<Move> legals = legal_moves(b);

        if (players_turn) {

            Move player_move;
            bool valid_move = false;

            while (!valid_move) {

                cout << ">> ";

                std::string userIn;
                cin >> userIn;

                player_move = stom(b, userIn);
                valid_move = !is_sentinel(player_move);

                if (!valid_move) {
                    cout << "Move not recognised or not legal\n";
                }
            }

            b = b.successor_hard(player_move);

        } else {

            const std::string fen = board_to_fen(b);

            cout << "Stase has " << seconds_per_move << " seconds to think\n";

            run_in_background(fen);
            sleep(seconds_per_move);
            stop_engine();

            const Move cmove = fetch_best_move();
            const int nodes = fetch_node_count();

            cout << "Running speed: " << ((double)nodes) / ((double)seconds_per_move) << " n/sec\n";

            bool valid = false;
            if (!is_sentinel(cmove)) {
                for (int i = 0; i < legals.size(); ++i) {
                    if (equal(cmove, legals[i])) {
                        valid = true;
                    }
                }
            }

            if (valid) {
                cout << "The computer played " << mtos(b, cmove) << "\n";
                b = b.successor_hard(cmove);
            } else {
                cout << "The computer tried to play an illegal move (from " << sqtos(cmove.from) << " to " << sqtos(cmove.to) << ").\n";
                cout << "You win!\n";
                keep_playing = false;
            }

        }

        if (in_check_hard(b) && legal_moves(b).empty()) {
            cout << "Checkmate!\n";
            cout << (players_turn
                        ? "You win!\n"
                        : "You lose!\n");
            keep_playing = false;
        } else {
            players_turn = !players_turn;
        }

    }

    cout << "Goodbye\n";

}

int main() {

    cout << welcome_message;

    signal(SIGSEGV, print_stack_trace);

    play_game(false, 10);

    pthread_exit(nullptr);

}
