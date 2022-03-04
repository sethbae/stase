#include <iostream>
using std::cout;

#include "board.h"
#include "game.h"
#include "glogic.h"
#include "../gamestate.hpp"

/*
 * print out a little grid of the control counts for each square
 */
void display_control_counts(const Gamestate & gs) {

    for (int y = 7; y >= 0; --y) {
        for (int x = 0; x < 8; ++x) {
            int count = gs.control_cache->get_control_count(mksq(x, y));

            std::string sign;
            if (count > 0) {
                sign = "+";
            } else if (count < 0) {
                sign = "";
            } else {
                sign = " ";
            }

            cout << sign << count << " ";
        }
        cout << "\n";
    }

}
