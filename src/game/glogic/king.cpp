#include "../../../include/stase/game.h"
#include "glogic.h"
#include "../gamestate.hpp"

inline bool is_safe_from_knights(const Gamestate & gs, const Square s, const Piece p) {

    Square temp;
    int x = s.x, y = s.y;

    if (val(temp = mksq(x + 1, y + 2)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x + 1, y - 2)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x + 2, y + 1)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x + 2, y - 1)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x - 1, y + 2)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x - 1, y - 2)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x - 2, y + 1)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x - 2, y - 1)) && gs.board.get(temp) == p) { return false; }

    return true;
}

inline bool is_safe_from_kings(const Gamestate & gs, const Square s, const Piece p) {

    Square temp;
    int x = s.x, y = s.y;

    if (val(temp = mksq(x + 1, y + 1)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x + 1, y))     && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x + 1, y - 1)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x, y - 1))     && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x, y + 1))     && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x - 1, y + 1)) && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x - 1, y))     && gs.board.get(temp) == p) { return false; }
    if (val(temp = mksq(x - 1, y - 1)) && gs.board.get(temp) == p) { return false; }

    return true;
}

/**
 * Looks for attackers of the opposite colour to the given king_colour by working outwards from
 * the square [s] along the given [delta]. Returns true if an attacker is found, false otherwise.
 */
bool seek_attackers(const Gamestate & gs, const Square s, const Colour king_colour, const Delta delta) {

    /**
     * NB this can be called from a sneaked context, so it is not safe to use
     * Gamestate::first_piece_encountered.
     */

    Square temp;
    const Piece ignored_king = (king_colour == WHITE) ? W_KING : B_KING;
    MoveType dir = direction_of_delta(delta);
    int x = get_x(s) + delta.dx, y = get_y(s) + delta.dy;

    while (val(temp = mksq(x, y))) {

        Piece p = gs.board.get(temp);

        if (p != EMPTY && p != ignored_king) {
            return colour(p) != king_colour && can_move_in_direction(p, dir);
        }

        x += delta.dx;
        y += delta.dy;
    }

    return false;
}

/**
 * Checks if a king of the given colour is safe on the given square. Does not assume that the king is currently
 * on the given square.
 */
int is_safe_for_king(const Gamestate & gs, const Square s, const Colour c) {

    Square temp;
    int x = get_x(s);
    int y = get_y(s);

    // knights
    if (!is_safe_from_knights(gs, s, (c == WHITE) ? B_KNIGHT : W_KNIGHT)) {
        return false;
    }

    // pawns
    if (c == WHITE) {
        if (val(temp = mksq(x + 1, y + 1)) && (gs.board.get(temp) == B_PAWN)) { return false; }
        if (val(temp = mksq(x - 1, y + 1)) && (gs.board.get(temp) == B_PAWN)) { return false; }
    } else {
        if (val(temp = mksq(x + 1, y - 1)) && (gs.board.get(temp) == W_PAWN)) { return false; }
        if (val(temp = mksq(x - 1, y - 1)) && (gs.board.get(temp) == W_PAWN)) { return false; }
    }

    // kings
    Piece enemy_king = (c == WHITE) ? B_KING : W_KING;
    if (!is_safe_from_kings(gs, s, enemy_king)) {
        return false;
    }

    // sliding directions
    for (int i = 0; i < 8; ++i) {
        if (seek_attackers(gs, s, c, Delta{(SignedByte) XD[i], (SignedByte) YD[i]})) {
            return false;
        }
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
    SquareControlStatus status = gs.control_cache->get_control_status(s);
    return colour == WHITE
        ? status.min_b > ATTACKED_BY_PINNED_PIECE
        : status.min_w > ATTACKED_BY_PINNED_PIECE;
}

/**
 * Checks whether the given-coloured king is on a safe square or not. This is functionally equivalent
 * to the in_check functions in the board module but may be implemented more efficiently. The value of
 * the duplication is that the slow and reliable variation can be used to check the correctness of the
 * performance-critical version.
 */
bool has_safe_king(const Gamestate & gs, const Colour king_colour) {
    return would_be_safe_king_square(gs, king_colour == WHITE ? gs.w_king : gs.b_king, king_colour);
}

/**
 * Checks whether a king of the given [colour] would be safe on the given square in the
 * same way as would_be_safe_king_square above, except that it treats the board as if
 * the given move had already been played.
 */
bool would_be_safe_for_king_after(const Gamestate & gs, const Square s, const Move m, Colour colour) {

    // sneak the move before this calculation
    Piece sneaked = gs.sneak(m);

    bool result = is_safe_for_king(gs, s, colour);

    gs.unsneak(m, sneaked);

    return result;
}
