#ifndef STASE_PIECE_ENCOUNTER_CACHE_HPP
#define STASE_PIECE_ENCOUNTER_CACHE_HPP

#include <iostream>
#include "board.h"
#include "../../board/board.hpp"

struct DirCache {
    Square p[8][8];
};

namespace __data {

    /**
     * For a given (dx, dy) delta, we store a list of squares. These are the squares we need to start from
     * when constructing the actual values in the cache. Because we work in the opposite direction to the
     * delta given, these values may look a little counter intuitive.
     * For example, for (-1, -1), the direction travels DOWN and LEFT. When constructing them in reverse,
     * we therefore need to work UP and RIGHT, from the left hand edge and the bottom edge, ie a8 -> a1 -> h1,
     * which is given in the first entry.
     * To index in, add 1 to each component of the delta: access dir_start_points[dx + 1][dy + 1][?].
     */
    extern const Square dir_start_points[3][3][16];

    /**
     * The store of actual caches; one for each direction. Index by (dx + 1, dy + 1).
     */
    extern DirCache caches[3][3];

    /**
     * A flag used internally to confirm that the caches are up to date. If set to false, they get recalculated
     * on next access.
     */
    extern bool caches_valid;

}

namespace __debug {
    inline void print_cache(DirCache &cache) {
        for (int y = 7; y >= 0; --y) {
            for (int x = 0; x < 8; ++x) {
                if (is_sentinel(cache.p[x][y])) {
                    std::cout << " . ";
                } else {
                    std::cout << sqtos(cache.p[x][y]) << " ";
                }
            }
            std::cout << "\n";
        }
    }
}

namespace __internal {
    inline void create_cache(const Board & b, DirCache & cache, Delta d) {

        for (int i = 0; i < 16; ++i) {

            Square start = __data::dir_start_points[d.dx + 1][d.dy + 1][i];
            int x = start.x, y = start.y;

            if (is_sentinel(start)) { break; }

            // every start square is next to the edge, so can never take a value
            cache.p[x][y] = SQUARE_SENTINEL;

            // if the start square is not empty, that's the current, otherwise it's still SENTINEL
            Square current;
            if (b.get(x, y) != EMPTY) {
                current = mksq(x, y);
            } else {
                current = SQUARE_SENTINEL;
            }

            // and move on once to the next square
            x -= d.dx;
            y -= d.dy;

            while (val(x, y)) {
                cache.p[x][y] = current;
                if (b.get(x, y) != EMPTY) {
                    current = mksq(x, y);
                }
                x -= d.dx;
                y -= d.dy;
            }

        }
    }
}

inline void create_piece_encountered_caches(const Board & b) {
    for (int i = 0; i < 8; ++i) {
        Delta d = delta(XD[i], YD[i]);
        __internal::create_cache(b, __data::caches[d.dx + 1][d.dy + 1], d);
    }
    __data::caches_valid = true;
}

inline Square __manual_first_piece_encountered(const Board & b, const Square start, const Delta delta) {

    int x = get_x(start) + delta.dx, y = get_y(start) + delta.dy;
    Square temp;

    while (val(temp = mksq(x, y)) && b.get(temp) == EMPTY) {
        x += delta.dx;
        y += delta.dy;
    }

    return val(temp) ? temp : SQUARE_SENTINEL;
}

/**
 * Walks out from the given square in the given direction until it reaches a piece.
 * It returns the square on which that piece lies, or SQUARE_SENTINEL if no piece was
 * encountered before the edge of the board.
 */
inline Square first_piece_encountered(const Board & b, const Square s, const Delta d) {
    if (!__data::caches_valid) {
        create_piece_encountered_caches(b);
    }
    return __data::caches[(d.dx + 1)][(d.dy + 1)].p[s.x][s.y];
}

#endif //STASE_PIECE_ENCOUNTER_CACHE_HPP
