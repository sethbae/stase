#ifndef STASE_PIECE_ENCOUNTER_CACHE_HPP
#define STASE_PIECE_ENCOUNTER_CACHE_HPP

#include <iostream>
#include "board.h"
#include "../../board/board.hpp"

struct DirCache {
    Square p[8][8];
};

struct PieceEncounteredCache {
    DirCache d[3][3];
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
 * Writes to the given PieceEncounteredCache so that it can be used to answer queries about the
 * board given.
 */
inline void  compute_cache(const Board & b, PieceEncounteredCache * cache) {
    for (int i = 0; i < 8; ++i) {
        Delta d = delta(XD[i], YD[i]);
        __internal::create_cache(b, cache->d[d.dx + 1][d.dy + 1], d);
    }
}

/**
 * Copies the contents of the first cache into the second.
 */
inline void copy_cache(const PieceEncounteredCache * from, PieceEncounteredCache * to) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int x = 0; x < 8; ++x) {
                for (int y = 0; y < 8; ++y) {
                    to->d[i][j].p[x][y] = from->d[i][j].p[x][y];
                }
            }
        }
    }
}

/**
 * Assuming that the cache reflects the current state of the board prior to this call, the update_cache
 * function modifies the new_cache so that it reflects the state of the board after the given move has been made.
 * The board should represent the position prior to the move being made.
 */
inline void update_cache(const Board & b, PieceEncounteredCache * cache, const Move m) {


    // from the from square of the move

    // for each direction

    // take the value on this square as the new value

    //                                  going in the opposite way
    // copy that value into the squares along that direction up to and including the first non-empty square

    Square target_sq = m.from;
    Square copy_val;

    for (int i = 0; i < 8; ++i) {

        Delta d = delta(XD[i], YD[i]);
        int x = target_sq.x - d.dx;
        int y = target_sq.y - d.dy;
        DirCache & dir_cache = cache->d[d.dx + 1][d.dy + 1];
        copy_val = dir_cache.p[target_sq.x][target_sq.y];

        while (val(x, y)) {
            dir_cache.p[x][y] = copy_val;
            if (b.get(x, y) != EMPTY) {
                break;
            } else {
                x -= d.dx;
                y -= d.dy;
            }
        }
    }

    target_sq = m.to;
    copy_val = m.to;

    for (int i = 0; i < 8; ++i) {

        Delta d = delta(XD[i], YD[i]);
        int x = target_sq.x - d.dx;
        int y = target_sq.y - d.dy;
        DirCache & dir_cache = cache->d[d.dx + 1][d.dy + 1];

        while (val(x, y)) {
            dir_cache.p[x][y] = copy_val;
            if (b.get(x, y) != EMPTY) {
                break;
            } else {
                x -= d.dx;
                y -= d.dy;
            }
        }
    }

    // from the to-square of the move

    // for each direction

    // take this piece as the new value

    // put that value in the squares (not the start square) leading away from that square (opposite to the delta)
    // up to and including the first non-empty square on the board.

}

#endif //STASE_PIECE_ENCOUNTER_CACHE_HPP
