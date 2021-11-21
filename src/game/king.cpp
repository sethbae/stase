#include "game.h"

/**
 * Looks for attackers of the opposite colour to the given king_colour by working outwards from
 * the square [s] along the given [delta]. Returns true if an attacker is found, false otherwise.
 */
bool seek_attackers(const Board & b, const Square s, const Colour king_colour, const DeltaPair delta) {

    Square temp;
    MoveType dir = direction_of_delta(delta);
    int x = get_x(s) + delta.xd, y = get_y(s) + delta.yd;

    while (val(temp = mksq(x, y))) {

        Piece p = b.get(temp);

        if (p != EMPTY) {
            return colour(p) != king_colour && can_move_in_direction(p, dir);
        }

        x += delta.xd;
        y += delta.yd;
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
        if (seek_attackers(gs.board, s, king_colour, DeltaPair{(Byte) XD[i], (Byte) YD[i]})) {
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

    Piece king = (colour == WHITE) ? W_KING : B_KING;
    Square k_sq;

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(mksq(x, y)) == king) {
                k_sq = mksq(x, y);
            }
        }
    }

    Piece sneaked_piece = gs.board.sneak(Move{k_sq, s});
    bool result = is_safe_for_king(gs, s);
    gs.board.unsneak(Move{k_sq, s}, sneaked_piece);

    return result;

}

/**
 * Checks whether a king of the given [colour] would be safe on the given square in the
 * same way as would_be_safe_king_square above, except that it treats the board as if
 * the given move had already been played.
 */
bool would_be_safe_for_king_after(const Gamestate & gs, const Square s, const Move m, Colour colour) {

    Piece sneaked_piece = gs.board.sneak(m);
    bool result = would_be_safe_king_square(gs, s, colour);
    gs.board.unsneak(m, sneaked_piece);

    return result;
}

/**
 * Checks whether the given-coloured king is on a safe square or not. This is functionally equivalent
 * to the in_check functions in the board module but may be implemented more efficiently. The value of
 * the duplication is that the slow and reliable variation can be used to check the correctness of the
 * performance-critical version.
 */
bool is_safe_king(const Gamestate & gs, const Colour king_colour) {
    Piece king = (king_colour == WHITE) ? W_KING : B_KING;
    Square k_sq;

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(mksq(x, y)) == king) {
                k_sq = mksq(x, y);
            }
        }
    }

    return is_safe_for_king(gs, k_sq);
}
