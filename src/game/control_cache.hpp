#ifndef STASE_CONTROL_CACHE_HPP
#define STASE_CONTROL_CACHE_HPP

#include "game.h"
#include "glogic/glogic.h"

/**
 * Caches a set of SquareControlStates by the square to which they refer.
 */
struct ControlCache {

    uint64_t squares = 0;
    SquareControlStatus cache[64];

    inline bool contains(const Square s) {
        return squares & (1l << index(s));
    }

    inline SquareControlStatus get(const Square s) {
        int ind = s.x + (8*s.y);
        return cache[ind];
    }

    inline void put(const Square s, SquareControlStatus status) {
        cache[index(s)] = status;
        squares |= (1l << index(s));
    }

    inline void clear() {
        squares = 0;
    }

private:
    constexpr unsigned index(const Square s) {
        return s.x + (8 * s.y);
    }
};

#endif //STASE_CONTROL_CACHE_HPP
