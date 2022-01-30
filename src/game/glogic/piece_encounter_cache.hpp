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
     * This array tracks whether the caches are valid or not.
     */
    extern bool valid[3][3];

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

/**
 * Sets all caches to be invalid, so that on access they will be refreshed.
 */
inline void invalidate_piece_encountered_caches() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            __data::valid[i][j] = false;
        }
    }
}

/**
 * Walks out from the given square in the given direction until it reaches a piece.
 * It returns the square on which that piece lies, or SQUARE_SENTINEL if no piece was
 * encountered before the edge of the board.
 */
inline Square first_piece_encountered(const Board & b, const Square s, const Delta d) {

    int i = d.dx + 1;
    int j = d.dy + 1;

    if (!__data::valid[i][j]) {
        __internal::create_cache(b, __data::caches[i][j], d);
        __data::valid[i][j] = true;
    }

    return __data::caches[i][j].p[s.x][s.y];
}

#endif //STASE_PIECE_ENCOUNTER_CACHE_HPP
