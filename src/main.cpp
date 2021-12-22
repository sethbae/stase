#include <iostream>
using std::cout;
using std::cin;

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <cstring>
#include <unistd.h>

#include <execinfo.h>
#include <signal.h>
#include <fstream>

#include "board.h"
#include "search.h"
#include "utils/utils.h"

const std::string GAME_FILE_DIR = "/home/seth/Documents/stase_lichess";

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

/**
 * Designed for handling a request which can be sent to the lichess API. This looks for
 * a file in the GAME_FILE_DIR called [game_id].game and reads in UCI moves from it.
 * Then it analyses the given position and replaces the file contents with the UCI for that move.
 * @param game_id the id of the game, used in the file name
 * @param seconds_per_move number of seconds to think for
 */
void handle_analysis_request(const std::string & game_id, const int seconds_per_move) {

    // open the file and read in the moves played
    std::ifstream file;
    const std::string file_path = GAME_FILE_DIR + "/" + game_id + ".game";
    file.open(file_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file) {
        std::cout << "Could not open file: " << GAME_FILE_DIR + "/" + game_id + ".game\n";
        return;
    }

    std::ifstream::pos_type file_size = file.tellg();
    std::vector<char> bytes(file_size);

    // read all the bytes into a string
    file.seekg(0, std::ios::beg);
    file.read(bytes.data(), file_size);
    file.close();
    const std::string moves_played(bytes.data(), file_size);

    // read the UCI into a vec of moves
    std::vector<Move> moves;

    int i = 0;
    while (i < moves_played.size()) {
        int j;
        for (j = i; j < moves_played.size() && moves_played[j] != ' '; ++j)
            ;

        // i...j is now a substring containing a single UCI move
        std::string uci = moves_played.substr(i, j-i);
        moves.push_back(uci2move(uci));

        i = j + 1;
    }

    // pull out the FEN of the position after all moves have been played
    Gamestate gs(starting_pos());
    for (const Move & m : moves) {
        gs.board = gs.board.successor_hard(m);
    }
    const std::string fen = board_to_fen(gs.board);

    // analyse the position
    run_in_background(fen);
    sleep(seconds_per_move);
    stop_engine();
    const Move move = fetch_best_move();

    // write the UCI back into the file
    std::ofstream file_out;
    file_out.open(file_path, std::fstream::out);
    file_out << move2uci(move);
    file_out.close();

}

int main(int argc, char** argv) {

    signal(SIGSEGV, print_stack_trace);

    bool interactive = true;
    int t = 10;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-g") == 0) {
            const std::string game_id(argv[++i]);
            handle_analysis_request(game_id, t);
            interactive = false;
        } else if (strcmp(argv[i], "-t") == 0) {
            const std::string time(argv[++i]);
            t = stoi(time);
        } else {
            cout << "Unrecognised argument: " + std::string(argv[i]) + "\n";
        }
    }

    if (interactive) {
        cout << welcome_message;
        play_game(false, t);
    }

    pthread_exit(nullptr);

}
