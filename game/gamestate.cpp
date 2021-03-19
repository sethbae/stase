#include "game.h"

/**
 *  This is the Gamestate object which is used to reduce calculations by storing some Bitmaps and other flags about the game.
 *  Each of those attributes should be updated or re-calculated every move.
 * 
 */

Gamestate::Gamestate() {

}

Gamestate::Gamestate(const Board & b) : board(b) {
    recalculate_all();
}

// With Ofast it takes about 5 microseconds
void Gamestate::recalculate_all() {
    Ptype fcolour = board.colour_to_move();
    Ptype ecolour = fcolour == WHITE ? BLACK : WHITE;

    vacancy = occupancy = foccupy = eoccupy = 0;
    kings = queens = bishops = knights = rooks = pawns = 0;

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {

            Square pos = mksq(i, j);
            Piece p = board.get(pos);

            if (type(p) != EMPTY) {
                if (colour(p) == WHITE) {
                    set_square(foccupy, pos);
                }

                set_square(occupancy, pos);

                switch (type(p)) {

                    case KING: set_square(kings, pos); break;
                    case QUEEN: set_square(queens, pos); break;
                    case BISHOP: set_square(bishops, pos); break;
                    case KNIGHT: set_square(knights, pos); break;
                    case ROOK: set_square(rooks, pos); break;
                    case PAWN: set_square(pawns, pos); break;
                    
                    default: break;
                }
            }

        }
    }

    vacancy = ~occupancy;
    eoccupy = ~foccupy & occupancy;

    fattack = attack_map(board, fcolour);
    eattack = attack_map(board, ecolour);
}
