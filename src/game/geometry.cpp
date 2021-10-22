#include "game.h"

/**
 * Checks whether the three points given all lie on a straight line.
 */
bool collinear_points(Square a, Square b, Square c) {
    return (get_y(a) - get_y(b)) * (get_x(b) - get_x(c)) == (get_y(b) - get_y(c)) * (get_x(a) - get_x(b));
}

DeltaPair get_delta_between(const Square a, const Square b) {
    int dx = get_x(b) - get_x(a);
    int dy = get_y(b) - get_y(a);
    if (dx != 0) {
        dx = (dx > 0) ? 1 : -1;
    }
    if (dy != 0) {
        dy = (dy > 0) ? 1 : -1;
    }
    return DeltaPair{ (Byte) dx, (Byte) dy};
}

/**
 * Checks whether the given board contains a path of empty squares which lie in a straight line from
 * [from] to [to]. If so, the required direction is returned.
 */
DeltaPair open_path_between(const Board & b, const Square from, const Square to) {

    int dx = get_x(to) - get_x(from);
    int dy = get_y(to) - get_y(from);
    Square temp;

    // orthogonal in the y direction
    if (dx == 0) {
        int delta = (dy > 0) ? 1 : -1;
        int x = get_x(from), y = get_y(from) + delta;

        while (val(temp = mksq(x, y)) && temp != to && b.get(temp) == EMPTY) {
            y += delta;
        }

        return (temp == to) ? DeltaPair{0, (Byte) delta} : INVALID_DELTA;
    }

    // orthogonal in the x direction
    if (dy == 0) {
        int delta = (dx > 0) ? 1 : -1;
        int x = get_x(from) + delta, y = get_y(from);

        while (val(temp = mksq(x, y)) && temp != to && b.get(temp) == EMPTY) {
            x += delta;
        }

        return (temp == to) ? DeltaPair{(Byte) delta, 0} : INVALID_DELTA;
    }

    // diagonal
    if (dx == dy) {
        int delta_x = (dx > 0) ? 1 : -1;
        int delta_y = (dy > 0) ? 1 : -1;
        int x = get_x(from) + delta_x, y = get_y(from) + delta_y;

        while (val(temp = mksq(x, y)) && temp != to && b.get(temp) == EMPTY) {
            x += delta_x;
            y += delta_y;
        }

        return (temp == to) ? DeltaPair{(Byte) delta_x, (Byte) delta_y} : INVALID_DELTA;
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

    if (line_start_point == line_end_point) { return false; }

    DeltaPair delta = get_delta_between(line_start_point, line_end_point);
    int x = get_x(line_start_point) + delta.xd, y = get_y(line_start_point) + delta.yd;

    Square temp;

    while (val(temp = mksq(x, y)) && temp != line_end_point) {
        if (alpha_covers(b, piece_sq, temp)) {
            return temp;
        }
        x += delta.xd;
        y += delta.yd;
    }

    return SQUARE_SENTINEL;

}
