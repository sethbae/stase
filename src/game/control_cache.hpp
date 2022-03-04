#ifndef STASE_CONTROL_CACHE_HPP
#define STASE_CONTROL_CACHE_HPP

#include "game.h"
#include "glogic/glogic.h"

/**
 * Caches a set of SquareControlStates by the square to which they refer.
 */
struct ControlCache {

    uint64_t squares = 0;
    SquareControlStatus status_cache[64];
    uint8_t control_count_cache[64];
    const Gamestate * gs;

    /**
     * If the square is already in the cache, this returns its status. Otherwise, it
     * computes and adds its status before returning it.
     */
    inline SquareControlStatus get_control_status(const Square s) {
        if (contains(s)) { return get_control(s); }
        SquareControlStatus status = evaluate_square_control(*gs, s);
        put_control(s, status);
        return status;
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

        squares = o.squares;
        for (int i = 0; i < 64; ++i) {
            status_cache[i] = o.status_cache[i];
            control_count_cache[i] = o.control_count_cache[i];
        }

        for (int i = 0; i < 64 && !is_sentinel(invalidated[i]); ++i) {
            squares &= ~(1l << index(invalidated[i]));
        }
    }

private:
    constexpr unsigned index(const Square s) {
        return s.x + (8 * s.y);
    }

    inline bool contains(const Square s) {
        return squares & (1l << index(s));
    }

    inline SquareControlStatus get_control(const Square s) {
        int ind = s.x + (8*s.y);
        return status_cache[ind];
    }

    inline void put_control(const Square s, SquareControlStatus status) {
        status_cache[index(s)] = status;
        squares |= (1l << index(s));
    }
};

#endif //STASE_CONTROL_CACHE_HPP
