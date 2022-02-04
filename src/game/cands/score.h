#ifndef STASE_SCORE_H
#define STASE_SCORE_H

#include "cands.h"
#include "../heur/heur.h"
#include "../../utils/utils.h"

constexpr int safe_check_score(const Piece p) {
    return 2;
}
constexpr int unsafe_check_score(const Piece p) {
    if (is_major_piece(p)) {
        return 0;
    } else {
        return 1;
    }
}

constexpr int capture_piece_score(const bool totally_undefended, const Piece capturing, const Piece captured) {
    if (totally_undefended) {
        return piece_value(captured) / 1000;
    }
    int diff = piece_value(captured) - piece_value(capturing);
    return max(2 + (diff / 1000), 0);
}

constexpr int fork_score(const int value_a, const int value_b) {
    return max(value_a / 1000, value_b / 1000);
}

constexpr int promotion_score() {
    return 8;
}

constexpr int pin_skewer_score(const int value_of_pinner, const int value_of_pinned, const int value_of_pinned_to) {
    int value_putatively_lost = min(value_of_pinned, value_of_pinned_to) - value_of_pinner;
    return max(value_putatively_lost / 1000, 1);
}

constexpr int defend_score(const Piece defended_piece) {
    if (is_major_piece(defended_piece)) {
        return 3;
    } else if (type(defended_piece) != PAWN) {
        return 2;
    } else {
        return 1;
    }
}

constexpr int trade_score(const Piece trade_this, const Piece for_this) {
    int diff = piece_value(for_this) - piece_value(trade_this);
    return 1 + (diff / 1000);
}

constexpr int retreat_score(Piece p) {
    return is_major_piece(p) ? 5 : 3;
}

constexpr int desperado_score(int piece_val) {
    return piece_val / 1000;
}

#endif //STASE_SCORE_H
