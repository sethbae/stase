#include "game.h"
#include "gamestate.hpp"

/**
 * This file implements a basic interface over gamestates so that you can use them in
 * basic ways without including gamestate.hpp.
 * Currently, this is more or less useless, because you still can't instantiate them.
 */

Gamestate * fresh_gamestate(const std::string & fen) {
    return new Gamestate(fen);
}

Gamestate * next_gamestate(const Gamestate * gs, const Move m) {
    return new Gamestate(*gs, m);
}
