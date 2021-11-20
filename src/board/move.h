#ifndef STASE_MOVE_H
#define STASE_MOVE_H

#include "board.h"

/* A move structure, which stores some flags etc and get/set methods */
struct Move {

    Square from;
    Square to;
    uint_fast16_t flags;

    bool is_prom() const;
    void set_prom();
    void unset_prom();

    bool is_cas() const;
    void set_cas();
    void unset_cas();

    bool is_ep() const;
    void set_ep();
    void unset_ep();

    bool is_cap() const;
    void set_cap();
    void unset_cap();

    bool is_cas_short() const;
    void set_cas_short();
    void unset_cas_short();

    int get_ep_file() const;

    Byte get_prom_shift() const;
    Piece get_prom_piece(Ptype) const;
    void set_prom_piece(Ptype);
    Piece get_cap_piece() const;
    void set_cap_piece(Piece p);

};

const Move MOVE_SENTINEL = Move{SQUARE_SENTINEL, 0, 0};

inline bool is_sentinel(const Move m) { return equal(m.from, SQUARE_SENTINEL); }
inline bool equal(const Move m1, const Move m2) {
    return equal(m1.from, m2.from) && equal(m1.to, m2.to);
}

#endif //STASE_MOVE_H
