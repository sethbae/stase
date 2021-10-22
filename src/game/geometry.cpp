#include "game.h"

/**
 * Checks whether the three points given all lie on a straight line.
 */
bool collinear_points(Square a, Square b, Square c) {
    return (get_y(a) - get_y(b)) * (get_x(b) - get_x(c)) == (get_y(b) - get_y(c)) * (get_x(a) - get_x(b));
}

/**
 * Checks whether the given board contains a path of empty squares which lie in a straight line from
 * [from] to [to]. If so, the required direction is returned.
 */
DeltaPair open_path_between(const Board & b, const Square from, const Square to, int xd, int yd) {

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
