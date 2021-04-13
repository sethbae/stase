#include <iostream>
using std::cout;
#include <vector>
using std::vector;

#include "board/board.h"
#include "game/game.h"
#include "search/search.h"

int main(void) {

    Gamestate gs;
    gs.board = starting_pos();
    
    vector<Move> vec;
    
    depth_limited_search(gs, 4);
        
    return 0;

}
