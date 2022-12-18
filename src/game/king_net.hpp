#ifndef STASE_KING_NET_HPP
#define STASE_KING_NET_HPP

#include "../board/board.hpp"
#include "../../include/stase/game.h"
#include "glogic/glogic.h"

class KingNet {

private:
    const Square k_sq;
    Square neighbours[8];
    bool safe[8];
    Byte n;

public:
    explicit KingNet(const Gamestate & gs, const Board & b, const Square k_sq) :
            k_sq(k_sq), n(0) {

        Colour c = colour(b.get(k_sq));

        for (int i = 0; i < 8; ++i) {
            int x = k_sq.x + XD[i], y = k_sq.y + YD[i];
            if (val(x, y)) {
                neighbours[i] = mksq(x, y);
                if (is_safe(gs, b, mksq(x, y), c)) {
                    safe[i] = true;
                    ++n;
                } else {
                    safe[i] = false;
                }
            } else {
                neighbours[i] = SQUARE_SENTINEL;
                safe[i] = false;
            }
        }
    }

private:
    /**
     * @return true if a king of the given colour could use the given square to avoid check.
     */
    static inline bool is_safe(const Gamestate & gs, const Board & b, const Square s, const Colour c) {

        Piece p = b.get(s);

        if (colour(p) == c) {
            return false;
        }
        if (p != EMPTY) {
            return totally_undefended(gs, colour(p), s);
        }
        return would_be_safe_king_square(gs, s, c);
    }

public:

    inline int flight_squares() const {
        return n;
    }
};

#endif //STASE_KING_NET_HPP
