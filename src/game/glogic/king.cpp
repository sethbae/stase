#include "game.h"
#include "board.h"
#include "glogic.h"
#include "../gamestate.hpp"

/**
 * Looks for attackers of the opposite colour to the given king_colour by working outwards from
 * the square [s] along the given [delta]. Returns true if an attacker is found, false otherwise.
 */
bool seek_attackers(const Board & b, const Square s, const Colour king_colour, const Delta delta) {

    Square temp;
    MoveType dir = direction_of_delta(delta);
    int x = get_x(s) + delta.dx, y = get_y(s) + delta.dy;

    while (val(temp = mksq(x, y))) {

        Piece p = b.get(temp);

        if (p != EMPTY) {
            return colour(p) != king_colour && can_move_in_direction(p, dir);
        }

        x += delta.dx;
        y += delta.dy;
    }

    return false;
}

/**
 * Checks if a king is safe, assuming that it is currently standing on the given square.
 * Does not allow for potentials (ie, "the king is currently on f2, would it be safe on e3?")
 */
int is_safe_for_king(const Gamestate & gs, const Square s) {

    Square temp;
    int x = get_x(s);
    int y = get_y(s);
    Colour king_colour = colour(gs.board.get(s));

    // sliding directions
    for (int i = 0; i < 8; ++i) {
        if (seek_attackers(gs.board, s, king_colour, Delta{(SignedByte) XD[i], (SignedByte) YD[i]})) {
            return false;
        }
    }

    // knights
    Piece enemy_knight = (king_colour == WHITE) ? B_KNIGHT : W_KNIGHT;
    if (val(temp = mksq(x + 1, y + 2)) && gs.board.get(temp) == enemy_knight)
        return false;
    if (val(temp = mksq(x + 1, y - 2)) && gs.board.get(temp) == enemy_knight)
        return false;
    if (val(temp = mksq(x + 2, y + 1)) && gs.board.get(temp) == enemy_knight)
        return false;
    if (val(temp = mksq(x + 2, y - 1)) && gs.board.get(temp) == enemy_knight)
        return false;
    if (val(temp = mksq(x - 1, y + 2)) && gs.board.get(temp) == enemy_knight)
        return false;
    if (val(temp = mksq(x - 1, y - 2)) && gs.board.get(temp) == enemy_knight)
        return false;
    if (val(temp = mksq(x - 2, y + 1)) && gs.board.get(temp) == enemy_knight)
        return false;
    if (val(temp = mksq(x - 2, y - 1)) && gs.board.get(temp) == enemy_knight)
        return false;

    // kings
    Piece enemy_king = (king_colour == WHITE) ? B_KING : W_KING;
    if (val(temp = mksq(x + 1, y + 1)) && (type(gs.board.get(temp)) == KING))
        return false;
    if (val(temp = mksq(x + 1, y + 0)) && (type(gs.board.get(temp)) == KING))
        return false;
    if (val(temp = mksq(x + 1, y - 1)) && (type(gs.board.get(temp)) == KING))
        return false;
    if (val(temp = mksq(x + 0, y + 1)) && (type(gs.board.get(temp)) == KING))
        return false;
    if (val(temp = mksq(x + 0, y - 1)) && (type(gs.board.get(temp)) == KING))
        return false;
    if (val(temp = mksq(x - 1, y + 1)) && (type(gs.board.get(temp)) == KING))
        return false;
    if (val(temp = mksq(x - 1, y + 0)) && (type(gs.board.get(temp)) == KING))
        return false;
    if (val(temp = mksq(x - 1, y - 1)) && (type(gs.board.get(temp)) == KING))
        return false;

    // pawns
    if (king_colour == WHITE) {
        if (val(temp = mksq(x + 1, y + 1)) && (gs.board.get(temp) == B_PAWN))
            return false;
        if (val(temp = mksq(x - 1, y + 1)) && (gs.board.get(temp) == B_PAWN))
            return false;
    } else {
        if (val(temp = mksq(x + 1, y - 1)) && (gs.board.get(temp) == W_PAWN))
            return false;
        if (val(temp = mksq(x - 1, y - 1)) && (gs.board.get(temp) == W_PAWN))
            return false;
    }

    return true;

}

/**
 * Checks whether in the current position, a king would be safe on the given square.
 * This is evaluated as if the king's next move were from its current location to the
 * square given, although note that there is no requirement for this move to be legal.
 * @param colour: the colour of the king to check this for.
 */
bool would_be_safe_king_square(const Gamestate & gs, const Square s, const Colour colour) {

    Square k_sq;
    if (colour == WHITE) {
        k_sq = gs.w_king;
    } else {
        k_sq = gs.b_king;
    }

    Piece sneaked = gs.sneak(Move{k_sq, s, 0});
    bool result = is_safe_for_king(gs, s);
    gs.unsneak(Move{k_sq, s, 0}, sneaked);

    return result;

}

/**
 * Checks whether a king of the given [colour] would be safe on the given square in the
 * same way as would_be_safe_king_square above, except that it treats the board as if
 * the given move had already been played.
 */
bool would_be_safe_for_king_after(const Gamestate & gs, const Square s, const Move m, Colour colour) {

    Piece sneaked = gs.sneak(m);
    bool result = would_be_safe_king_square(gs, s, colour);
    gs.unsneak(m, sneaked);

    return result;
}

/**
 * Checks whether the given-coloured king is on a safe square or not. This is functionally equivalent
 * to the in_check functions in the board module but may be implemented more efficiently. The value of
 * the duplication is that the slow and reliable variation can be used to check the correctness of the
 * performance-critical version.
 */
bool is_safe_king(const Gamestate & gs, const Colour king_colour) {

    Square k_sq;
    if (king_colour == WHITE) {
        k_sq = gs.w_king;
    } else {
        k_sq = gs.b_king;
    }

    return is_safe_for_king(gs, k_sq);
}