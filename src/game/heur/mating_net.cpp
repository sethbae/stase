#include "heur.h"
#include "../king_net.hpp"
#include "../gamestate.hpp"

float score(const KingNet * king_net) {
    return ((float) (8 - king_net->flight_squares())) / 8.0f;
}

float __metrics::__mating_net(const Gamestate & gs) {
    return gs.board.get_white()
        ? 1 * score(gs.b_king_net)
        : -1 * score(gs.w_king_net);
}
