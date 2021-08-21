#include "board.h"
#include "game.h"

#include <vector>
using std::vector;

// return candidate moves for a board; as a minimal working example,
// return all legal moves
vector<Move> cands(const Gamestate & gs) {
    
    vector<Move> v;
    legal_moves(gs.board, v);
    return v;

}
