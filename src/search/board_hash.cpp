#include "search_tools.h"
#include "../game/gamestate.hpp"

long SearchNode::hash() {

    long hash = 0l;
    long mask = 1l;

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs->board.get(x, y) != EMPTY) {
                hash |= mask;
            }
            mask <<= 1;
        }
    }

    return hash;

}
