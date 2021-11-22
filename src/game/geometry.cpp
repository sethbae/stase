#include "game.h"

/**
 * Checks whether the three points given all lie on a straight line.
 */
bool collinear_points(Square a, Square b, Square c) {
    return (get_y(a) - get_y(b)) * (get_x(b) - get_x(c)) == (get_y(b) - get_y(c)) * (get_x(a) - get_x(b));
}

Delta get_delta_between(const Square a, const Square b) {
    SignedByte dx = b.x - a.x;
    SignedByte dy = b.y - a.y;
    if (dx != 0) {
        dx = (dx > 0) ? 1 : -1;
    }
    if (dy != 0) {
        dy = (dy > 0) ? 1 : -1;
    }
    return Delta{dx, dy};
}

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
 * Checks whether the piece at the given square can move onto the line which runs from [line_start_point]
 * to [line_end_point]. If it can move onto it, then the square which it can move onto is returned, otherwise
 * SQUARE_SENTINEL is returned.
 * If there are multiple squares on the line which the piece can move to, then the first encountered is returned.
 */
Square can_move_onto_line(
        const Board & b, const Square piece_sq, const Square line_start_point, const Square line_end_point) {

    if (equal(line_start_point, line_end_point)) { return SQUARE_SENTINEL; }

    Delta delta = get_delta_between(line_start_point, line_end_point);
    int x = get_x(line_start_point) + delta.dx, y = get_y(line_start_point) + delta.dy;

    Square temp;

    while (val(temp = mksq(x, y)) && !equal(temp, line_end_point)) {
        if (alpha_covers(b, piece_sq, temp)) {
            return temp;
        }
        x += delta.dx;
        y += delta.dy;
    }

    return SQUARE_SENTINEL;

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
