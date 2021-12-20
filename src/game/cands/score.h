#ifndef STASE_SCORE_H
#define STASE_SCORE_H

#include "cands.h"

constexpr int safe_check_score(const Piece p) {
    return 0;
}
constexpr int unsafe_check_score(const Piece p) {
    return 0;
}

constexpr int capture_piece_score(const Piece capturing, const Piece captured) {
    return 0;
}

constexpr int fork_score(const int value_a, const int value_b) {
    return 0;
}

constexpr int promotion_score() {
    return 0;
}

constexpr int pin_skewer_score(const Piece p) {
    return 0;
}

constexpr int trade_score(const Piece trade_this, const Piece for_this) {
    return 0;
}

constexpr int retreat_score() {
    return 0;
}

constexpr int desperado_score(int piece_val) {
    return 0;
}

#endif //STASE_SCORE_H
