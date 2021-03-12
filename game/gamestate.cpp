#include "board.h"

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

    // These take about 4.2 microseconds
    vacancy = vacancy_map(board);
    occupancy = ~vacancy;
    foccupy = friendly_map(board);
    eoccupy = occupancy & ~foccupy;
    fattack = attack_map(board, fcolour);
    eattack = attack_map(board, ecolour);

    // Locations of all pieces of a certain type (can be ANDed with occupation to isolate pieces of desired colour)
    // These take about 5 microseconds to calculate
    kings = custom_map(board, [] (const Board & b, Square s) { return type(b.get(s)) == KING; });
    pawns = custom_map(board, [] (const Board & b, Square s) { return type(b.get(s)) == PAWN; });
    queens = custom_map(board, [] (const Board & b, Square s) { return type(b.get(s)) == QUEEN; });
    knights = custom_map(board, [] (const Board & b, Square s) { return type(b.get(s)) == KNIGHT; });
    bishops = custom_map(board, [] (const Board & b, Square s) { return type(b.get(s)) == BISHOP; });
    rooks = custom_map(board, [] (const Board & b, Square s) { return type(b.get(s)) == ROOK; });
}
