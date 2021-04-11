#ifndef SEARCH_H
#define SEARCH_H

#include "../board/board.h"
#include "../game/game.h"

std::vector<Move> depth_limited_search(const Gamestate &, int);


#endif
