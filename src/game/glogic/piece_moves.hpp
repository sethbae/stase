#ifndef STASE_PIECE_MOVES_HPP
#define STASE_PIECE_MOVES_HPP

#include "glogic.h"
#include "../../../include/stase/board.h"

namespace __bools {
    const bool PIECE_MOVE_TYPES[][6] = {
            { false, false, false, false, true, false },    // KING
            { true, true, false, false, false, false },     // QUEEN
            { true, false, false, false, false, false },    // ROOK
            { false, false, true, false, false, false },    // KNIGHT
            { false, true, false, false, false, false },    // BISHOP
            { false, false, false, true, false, false },    // PAWN
            { false, false, false, false, false, false },   // ---------invalid-----------
            { false, false, false, false, false, false },   // ---------invalid-----------
            { false, false, false, false, true, false },    // KING
            { true, true, false, false, false, false },     // QUEEN
            { true, false, false, false, false, false },    // ROOK
            { false, false, true, false, false, false },    // KNIGHT
            { false, true, false, false, false, false },    // BISHOP
            { false, false, false, true, false, false },    // PAWN
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // ---------invalid-----------
            { false, false, false, false, false, false },    // EMPTY
            { false, false, false, false, false, false }     // INVALID
    };
}

inline bool can_move_in_direction(Piece p, MoveType dir) {
    return __bools::PIECE_MOVE_TYPES[p][dir];
}
inline bool can_move_in_direction(Piece p, Delta d) {
    return can_move_in_direction(p, direction_of_delta(d));
}

#endif //STASE_PIECE_MOVES_HPP
