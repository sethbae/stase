#ifndef STASE_SCORE_H
#define STASE_SCORE_H

#include "cands.h"
#include "../heur/heur.h"
#include "../../utils/utils.h"

constexpr int safe_check_score(const Piece p) {
    return 4;
}
constexpr int unsafe_check_score(const Piece p) {
    if (is_major_piece(p)) {
        return 1;
    } else {
        return 2;
    }
}

constexpr int capture_piece_score(const Piece capturing, const Piece captured) {
    int diff = piece_value(captured) - piece_value(capturing);
    return max(2 + (diff / 1000), 0);
}

constexpr int fork_score(const int value_a, const int value_b) {
    return max(value_a / 1000, value_b / 1000);
}

constexpr int promotion_score() {
    return 8;
}

constexpr int pin_skewer_score(const Piece p) {
    return max((piece_value(p) / 1000) - 2, 1);
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
