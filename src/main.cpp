#include <iostream>
using std::cout;
using std::cin;

#include <vector>
using std::vector;
#include <string>
using std::string;

#include "board.h"

std::string welcome_message =
        "Welcome to Stase v4.0\n";

void manual_gameplay() {

    Board b = starting_pos();
    vector<Move> moves;

    while (true) {

        pr_board_conf(b); cout << "\n";

        moves.clear();
        legal_moves(b, moves);

        for (Move m : moves) {
            cout << movetosan(b, m) << " ";
        }
        cout << "\n\n";

        Move player_move = empty_move();
        bool valid_move = false;

        while (!valid_move) {

            cout << ">> ";

            string userIn;
            cin >> userIn;

            for (Move m : moves) {
                if (movetosan(b, m) == userIn) {
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

int main() {

    cout << welcome_message;

    manual_gameplay();

    return 0;
}
