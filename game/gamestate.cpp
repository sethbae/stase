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

void Gamestate::recalculate_attacks() {
    Ptype fcolour = board.colour_to_move();
    Ptype ecolour = fcolour == WHITE ? BLACK : WHITE;

    fattack = attack_map(board, fcolour);
    eattack = attack_map(board, ecolour);
}

void Gamestate::recalculate_positions() {
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
}

// With Ofast it takes about 5 microseconds
void Gamestate::recalculate_all() {
    recalculate_positions();
    recalculate_attacks();
}

Bitmap invalid = -1;
Bitmap & get_type_map(Gamestate & g, Ptype t) {
    switch(t) {
        case KING: return g.kings;
        case QUEEN: return g.queens;
        case BISHOP: return g.bishops;
        case KNIGHT: return g.knights;
        case ROOK: return g.rooks;
        case PAWN: return g.pawns;
        default: return invalid;
    }
}

void Gamestate::update(Move m) {
    Piece p_from = board.get(m.from);
    Piece p_to = board.get(m.to);

    Bitmap & from_map = get_type_map(* this, type(p_from));
    Bitmap & to_map = get_type_map(* this, type(p_to));

    occupancy &= ~foccupy;

    unset_square(from_map, m.from);
    set_square(from_map, m.to);

    unset_square(foccupy, m.from);
    set_square(foccupy, m.to);

    if (to_map != invalid) {
        unset_square(to_map, m.to);
        unset_square(eoccupy, m.to);
    }

    occupancy |= foccupy;
    vacancy = ~occupancy;
}

bool in_check(Gamestate & g) {
    return g.kings & g.foccupy & g.eattack != 0;
}