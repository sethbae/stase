#include <iostream>
using std::cout;

#include "board/board.h"
#include "game/game.h"

int main(void) {

    Gamestate gs;
    gs.board = starting_pos();
    
    cout << heur(gs) << "\n";
    
    return 0;

}
