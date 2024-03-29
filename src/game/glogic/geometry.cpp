#include "../../../include/stase/game.h"
#include "board.h"
#include "glogic.h"
#include "../../board/board.hpp"
#include "../gamestate.hpp"

/**
 * Checks whether the piece at the given square can move onto the line which runs from [line_start_point] (exclusive)
 * to [line_end_point] (inclusive). If it can move onto it, then the square which it can move onto is returned, otherwise
 * SQUARE_SENTINEL is returned.
 * If there are multiple squares on the line which the piece can move to, then the first encountered is returned.
 *
 * @param piece_sq the square of the piece to check
 * @param line_start_point the start square of the line - NOT INCLUDED
 * @param line_end_point the end square of the line - POTENTIALLY INCLUDED
 */
Square square_piece_can_reach_on_line(
        const Board & b, const Square piece_sq, const Square line_start_point, const Square line_end_point) {

    if (equal(line_start_point, line_end_point)) { return SQUARE_SENTINEL; }

    Delta delta = get_delta_between(line_start_point, line_end_point);
    int x = get_x(line_start_point) + delta.dx, y = get_y(line_start_point) + delta.dy;

    Square temp;

    while (val(temp = mksq(x, y))) {
        if (alpha_covers(b, piece_sq, temp)) {
            return temp;
        }
        if (equal(temp, line_end_point)) {
            return SQUARE_SENTINEL;
        }
        x += delta.dx;
        y += delta.dy;
    }

    return SQUARE_SENTINEL;

}

/**
 * Checks whether the piece at the given square can move onto the line which runs from [line_start_point] (exclusive)
 * to [line_end_point] (inclusive). If it can move onto it, then a list of the squares on it can move to is returned.
 *
 * This should be used for queens, kings or knights. Other pieces can generally only move onto one square on any given
 * line (along which they can move).
 *
 * @param piece_sq the square of the piece to check
 * @param line_start_point the (excluded) start point of the line to move on to
 * @param line_end_point the (included) end point of the line to move on to
 */
void squares_piece_can_reach_on_line(
        const Board & b, ptr_vec<Square> & squares, const Square piece_sq, const Square line_start_point, const Square line_end_point) {

    if (equal(line_start_point, line_end_point)) { return; }

    Colour p_colour = colour(b.get(piece_sq));
    Delta delta = get_delta_between(line_start_point, line_end_point);
    int x = get_x(line_start_point) + delta.dx, y = get_y(line_start_point) + delta.dy;

    Square temp;

    while (val(temp = mksq(x, y))) {
        // check beta covers so that captures are included, but don't consider captures of your own colour
        if (beta_covers(b, piece_sq, temp) && colour(b.get(temp)) != p_colour) {
            squares.push(mksq(x, y));
        }
        if (equal(temp, line_end_point)) {
            return;
        }
        x += delta.dx;
        y += delta.dy;
    }
}

/**
 * Finds squares moving in one direction according to gamma-logic. Writes to squares starting from the given
 * index.
 */
inline void gamma_explore(const Board & b, const Square s, ptr_vec<Square> & squares, Delta d, MoveType dir) {

    Colour c = colour(b.get(s));
    Square temp = s;
    bool cont = true;

    temp.x += d.dx;
    temp.y += d.dy;

    while (val(temp) && cont) {

        squares.push(temp);

        Piece otherp = b.get(temp);
        if (otherp != EMPTY) {
            // gamma: x-ray
            cont = (colour(otherp) == c) && can_move_in_direction(otherp, dir);
        }
        temp.x += d.dx;
        temp.y += d.dy;
    }
}

/**
 * Finds squares which the given move might invalidate information about (eg, for the control_cache).
 * This is defined as all of the following:
 *   - squares gamma-reachable from the start or end square of the move
 *   - if the start move contains a knight, squares a knight move from either start or end square.
 *   - if the end move contains a knight, squares a knight move from the end square.
 * Board state should be prior to the playing of the move.
 * The squares pointer should have space for 80 (=64 + 8 + 8) squares.
 */
void find_invalidated_squares(const Board & b, const Move m, ptr_vec<Square> & squares) {

    for (const Delta d : D) {
        gamma_explore(b, m.from, squares, d, direction_of_delta(d));
        gamma_explore(b, m.to, squares, d, direction_of_delta(d));
    }

    if (type(b.get(m.from)) == KNIGHT) {
        for (int j = 0; j < 8; ++j) {
            Square temp = mksq(m.from.x + XKN[j], m.from.y + YKN[j]);
            if (val(temp)) {
                squares.push(temp);
            }
            temp = mksq(m.to.x + XKN[j], m.to.y + YKN[j]);
            if (val(temp)) {
                squares.push(temp);
            }
        }
    }

    else if (type(b.get(m.to)) == KNIGHT) {
        for (int j = 0; j < 8; ++j) {
            Square temp = mksq(m.to.x + XKN[j], m.to.y + YKN[j]);
            if (val(temp)) {
                squares.push(temp);
            }
        }
    }
}
