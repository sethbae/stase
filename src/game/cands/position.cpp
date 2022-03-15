#include "cands.h"
#include "hook.hpp"
#include "../gamestate.hpp"

bool positional_move_hook(Gamestate & gs, const Square s) {
    if (is_not_pk(gs.board.get(s))) {
        return gs.add_frame(positional_hook.id, FeatureFrame{s, SQUARE_SENTINEL, 0, 0});
    }
    return true;
}

const Hook positional_hook{
    "position",
    9,
    &positional_move_hook
};
