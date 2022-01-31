#include "piece_encounter_cache.hpp"

/**
 * For a given (dx, dy) delta, we store a list of squares. These are the squares we need to start from
 * when constructing the actual values in the cache. Because we work in the opposite direction to the
 * delta given, these values may look a little counter intuitive.
 * For example, for (-1, -1), the direction travels DOWN and LEFT. When constructing them in reverse,
 * we therefore need to work UP and RIGHT, from the left hand edge and the bottom edge, ie a8 -> a1 -> h1,
 * which is given in the first entry.
 * To index in, add 1 to each component of the delta: access dir_start_points[dx + 1][dy + 1][?].
 */
const Square __data::dir_start_points[3][3][16] = {
        {
                // (-1, -1): a8 -> a1 -> h1
                {stosq("a8"), stosq("a7"), stosq("a6"), stosq("a5"), stosq("a4"), stosq("a3"), stosq("a2"), stosq(
                        "a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq(
                        "h1"), SQUARE_SENTINEL},

                // (-1, 0): a1 -> a8
                {stosq("a1"), stosq("a2"), stosq("a3"), stosq("a4"), stosq("a5"), stosq("a6"), stosq("a7"), stosq(
                        "a8"), SQUARE_SENTINEL},

                // (-1, +1): a1 -> a8 -> h8
                {stosq("a1"), stosq("a2"), stosq("a3"), stosq("a4"), stosq("a5"), stosq("a6"), stosq("a7"), stosq(
                        "a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq(
                        "h8"), SQUARE_SENTINEL},

        },

        {
                // (0, -1): a1 -> h1
                {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq(
                        "h1"), SQUARE_SENTINEL},

                // (0, 0)
                {SQUARE_SENTINEL},

                // (0, +1): a8 -> h8
                {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq(
                        "h8"), SQUARE_SENTINEL},

        },

        {
                // (1, -1): a1 -> h1 -> h8
                {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq(
                        "h1"), stosq("h2"), stosq("h3"), stosq("h4"), stosq("h5"), stosq("h6"), stosq("h7"), stosq(
                        "h8"), SQUARE_SENTINEL},

                // (1, 0): h1 -> h8
                {stosq("h1"), stosq("h2"), stosq("h3"), stosq("h4"), stosq("h5"), stosq("h6"), stosq("h7"), stosq(
                        "h8"), SQUARE_SENTINEL},

                // (1, +1): a8 -> h8 -> h1
                {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq(
                        "h8"), stosq("h7"), stosq("h6"), stosq("h5"), stosq("h4"), stosq("h3"), stosq("h2"), stosq(
                        "h1"), SQUARE_SENTINEL},
        },
};

/**
 * The store of actual caches; one for each direction. Index by (dx + 1, dy + 1).
 */
DirCache __data::caches[3][3] = {
        {
                DirCache{},
                DirCache{},
                DirCache{},
        },
        {
                DirCache{},
                DirCache{},
                DirCache{},
        },
        {
                DirCache{},
                DirCache{},
                DirCache{},
        },
};

/**
 * A flag used internally to confirm that the caches are up to date. If set to false, they get recalculated
 * on next access.
 */
bool __data::caches_valid = false;
