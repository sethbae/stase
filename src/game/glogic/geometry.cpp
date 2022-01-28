#include "game.h"
#include "board.h"
#include "glogic.h"
#include "../../board/board.hpp"

/**
 * Checks whether the given board contains a path of empty squares which lie in a straight line from
 * [from] to [to]. If so, the required direction is returned.
 */
Delta open_path_between(const Board & b, const Square from, const Square to) {

    int dx = get_x(to) - get_x(from);
    int dy = get_y(to) - get_y(from);
    Square temp;

    // orthogonal in the y direction
    if (dx == 0) {
        int delta = (dy > 0) ? 1 : -1;
        int x = get_x(from), y = get_y(from) + delta;

        while (val(temp = mksq(x, y)) && !equal(temp, to) && b.get(temp) == EMPTY) {
            y += delta;
        }

        return equal(temp, to) ? Delta{0, (SignedByte) delta} : INVALID_DELTA;
    }

    // orthogonal in the x direction
    if (dy == 0) {
        int delta = (dx > 0) ? 1 : -1;
        int x = get_x(from) + delta, y = get_y(from);

        while (val(temp = mksq(x, y)) && !equal(temp, to) && b.get(temp) == EMPTY) {
            x += delta;
        }

        return equal(temp, to) ? Delta{(SignedByte) delta, 0} : INVALID_DELTA;
    }

    // diagonal
    if (dx == dy) {
        int delta_x = (dx > 0) ? 1 : -1;
        int delta_y = (dy > 0) ? 1 : -1;
        int x = get_x(from) + delta_x, y = get_y(from) + delta_y;

        while (val(temp = mksq(x, y)) && !equal(temp, to) && b.get(temp) == EMPTY) {
            x += delta_x;
            y += delta_y;
        }

        return equal(temp, to) ? Delta{(SignedByte) delta_x, (SignedByte) delta_y} : INVALID_DELTA;
    }

    return INVALID_DELTA;
}

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
std::vector<Square> squares_piece_can_reach_on_line(
        const Board & b, const Square piece_sq, const Square line_start_point, const Square line_end_point) {

    if (equal(line_start_point, line_end_point)) { return std::vector<Square>(); }

    Delta delta = get_delta_between(line_start_point, line_end_point);
    int x = get_x(line_start_point) + delta.dx, y = get_y(line_start_point) + delta.dy;

    Square temp;
    std::vector<Square> squares;

    while (val(temp = mksq(x, y))) {
        if (alpha_covers(b, piece_sq, temp)) {
            squares.push_back(mksq(x, y));
        }
        if (equal(temp, line_end_point)) {
            return squares;
        }
        x += delta.dx;
        y += delta.dy;
    }

    return squares;

}

/**
 * Walks out from the given square in the given direction until it reaches a piece.
 * It returns the square on which that piece lies, or SQUARE_SENTINEL if no piece was
 * encountered before the edge of the board.
 */
Square first_piece_encountered(const Board & b, const Square start, const Delta delta) {

    int x = get_x(start) + delta.dx, y = get_y(start) + delta.dy;
    Square temp;

    while (val(temp = mksq(x, y)) && b.get(temp) == EMPTY) {
        x += delta.dx;
        y += delta.dy;
    }

    return val(temp) ? temp : SQUARE_SENTINEL;

}
