#include <iostream>
using std::cout;
using std::cin;

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <cstring>

#include <signal.h>
#include <fstream>

#include "board.h"
#include "search.h"
#include "utils/utils.h"
#include "game/gamestate.hpp"
#include "search/engine.h"

const std::string welcome_message =
        "Welcome to Stase v4.0\n";

void manual_gameplay() {

    Board b = Board::starting_pos();
    vector<Move> moves;

    while (true) {

        board_utils::pr_board_conf(b); cout << "\n";

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

void play_game(const std::string & starting_fen, bool engine_is_white, int seconds_per_move) {

    Board b = board_utils::fen_to_board(starting_fen);
    bool players_turn = !engine_is_white;
    bool keep_playing = true;

    while (keep_playing) {

        board_utils::print(b);
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

            const std::string fen = board_utils::board_to_fen(b);

            cout << "Stase has " << seconds_per_move << " seconds to think\n";

            Engine engine =
                EngineBuilder::for_position(fen)
                    .with_timeout(seconds_per_move)
                    .build();
            engine.blocking_run();

            const Move cmove = engine.get_best_move();
            const int nodes = engine.get_nodes_explored();

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

int main(int argc, char** argv) {

    signal(SIGSEGV, print_stack_trace_and_abort);
    signal(SIGABRT, print_stack_trace_and_abort);

    int t = 10;
    std::string fen = starting_fen();

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-t") == 0) {
            const std::string time(argv[++i]);
            t = stoi(time);
        } else if (strcmp(argv[i], "-f") == 0) {
            const std::string quoted_fen = std::string(argv[++i]);
            fen = quoted_fen.substr(1, quoted_fen.size() - 1);
        } else {
            cout << "Unrecognised argument: " + std::string(argv[i]) + "\n";
        }
    }

    cout << welcome_message;
    play_game(fen, false, t);

    pthread_exit(nullptr);

}
