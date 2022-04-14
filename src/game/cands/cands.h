#ifndef STASE_CANDS_H
#define STASE_CANDS_H

#include "../../../include/stase/game.h"
#include "../../../include/stase/board.h"
#include "score.h"
#include <iostream>

const int MAX_TOTAL_CANDS = 30;
const int MAX_MOVES_PER_HOOK = 20;
const int MAX_MOVES_PER_FRAME = 5;

struct FeatureFrame {
    Square centre;
    Square secondary;
    int conf_1;
    int conf_2;
};

/** finds squares which the first square's piece can move to and cover the second square */
void find_sliding_cover_squares(
        const Gamestate &, std::vector<Square> &, const Square piece_square, const Square cover_square);

#endif //STASE_CANDS_H
