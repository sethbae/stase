#ifndef STASE_CONTROL_CACHE_HPP
#define STASE_CONTROL_CACHE_HPP

#include "game.h"
#include "glogic/glogic.h"

/**
 * Caches a set of SquareControlStates by the square to which they refer.
 */
struct ControlCache {

    uint64_t status_present = 0;
    SquareControlStatus status_cache[64];

    uint64_t count_present = 0;
    int8_t control_count_cache[64];

    const Gamestate * gs;

    /**
     * If the square is already in the cache, this returns its status. Otherwise, it
     * computes and adds its status before returning it.
     */
    inline SquareControlStatus get_control_status(const Square s) {
        if (contains_status(s)) { return get_status(s); }
        SquareControlStatus status = evaluate_square_control(*gs, s);
        put_status(s, status);
        return status;
    }

    inline int8_t get_control_count(const Square s) {
        if (contains_count(s)) { return get_count(s); }
        int count = control_count(*gs, s);
        put_count(s, count);
        return count;
    }

    /**
     * Copies the data from the given control cache, and marks invalid squares gamma-reachable
     * from the start or end square of the given move.
     * The given board may or may not belong to the gamestate which owns this control cache. It
     * should reflect the state of the board prior to the given move being made.
     */
    inline void update(const Board & b, const ControlCache & o, const Move m) {

        Square invalidated[64];
        find_invalidated_squares(b, &invalidated[0], m);

        status_present = o.status_present;
        count_present = o.count_present;
        for (int i = 0; i < 64; ++i) {
            status_cache[i] = o.status_cache[i];
            control_count_cache[i] = o.control_count_cache[i];
        }

        for (int i = 0; i < 64 && !is_sentinel(invalidated[i]); ++i) {
            const uint64_t MASK = ~(1l << index(invalidated[i]));
            status_present &= MASK;
            count_present &= MASK;
        }
    }

private:
    constexpr unsigned index(const Square s) {
        return s.x + (8 * s.y);
    }

    inline bool contains_status(const Square s) {
        return status_present & (1l << index(s));
    }

    inline bool contains_count(const Square s) {
        return count_present & (1l << index(s));
    }

    inline SquareControlStatus get_status(const Square s) {
        return status_cache[index(s)];
    }

    inline int8_t get_count(const Square s) {
        return control_count_cache[index(s)];
    }

    inline void put_status(const Square s, SquareControlStatus status) {
        status_cache[index(s)] = status;
        status_present |= (1l << index(s));
    }

    inline void put_count(const Square s, int8_t count) {
        control_count_cache[index(s)] = count;
        count_present |= (1l << index(s));
    }
};

#endif //STASE_CONTROL_CACHE_HPP
