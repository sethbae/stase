#include "cands.h"
#include "hook.hpp"

bool positional_move_hook(Gamestate & gs, const Square) {
    return true;
}

const Hook positional_hook{
    "position",
    9,
    &positional_move_hook
};
