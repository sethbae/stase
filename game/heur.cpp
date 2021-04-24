
#include "game.h"
#include "../board/board.h"

typedef int Metric(const Board &);

const Metric metrics[] = {
    
    

};

const int weights[] = {


};

// returns a positive integer representing the value of the piece
int piece_value(const Piece p) {
    
    switch (type(p)) {
        case PAWN: return 1;
        case KNIGHT:
        case BISHOP: return 3;
        case ROOK: return 5;
        default: return 9;
    }   
    
}

// returns an evaluation of the given board
Eval heur(const Gamestate & gs) {

    Eval count = (Eval) 0;
    
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Piece p = gs.board.get(mksq(x, y));
            
            if (colour(p) == WHITE)
                count += piece_value(p);
            else if (colour(p) == BLACK)
                count -= piece_value(p);
            // colour can be INVALID
        
        }
    }
    
    return count;

}

int piece_activity(const Board & b) {
    return 0;
}

int centre_control(const Board & b) {
    return 0;
}

int weak_pawns(const Board & b) {
    return 0;
}

int space(const Board & b) {
    return 0;
}

int king_safety(const Board & b) {
    return 0;
}

int development(const Board & b) {
    return 0;
}

