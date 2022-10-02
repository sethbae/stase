#include "search_tools.h"
#include "../game/gamestate.hpp"

long SearchNode::hash() {
    return ::board_hash(gs->board);
}

long board_hash(const Board & b) {
    long hash = 0l;
    long mask = 1l;

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (b.get(x, y) != EMPTY) {
                hash |= mask;
            }
            mask <<= 1;
        }
    }

    return hash;
}
