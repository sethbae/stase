#include "cands.h"
#include "hook.hpp"

bool discovered_attack_hook(Gamestate & gs, const Square s) {
    return false;
}

const Hook discovered_hook{
    "discovered",
    8,
    &discovered_attack_hook
};
