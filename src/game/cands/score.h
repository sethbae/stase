#ifndef STASE_SCORE_H
#define STASE_SCORE_H

#include "cands.h"
#include "../heur/heur.h"
#include "../../utils/utils.h"

constexpr int safe_check_score([[maybe_unused]] const Piece p) {
    return 2;
}
constexpr int unsafe_check_score(const Piece p) {
    if (is_major_piece(p)) {
        return 0;
    } else {
        return 1;
    }
}

constexpr int capture_piece_score(const bool totally_undefended, [[maybe_unused]] const Piece capturing, const Piece captured) {
    if (totally_undefended) {
        return piece_value(captured);
    }
    return 5;
}

constexpr int fork_score(const int value_a, const int value_b) {
    return max(value_a, value_b);
}

constexpr int promotion_score() {
    return 8;
}

constexpr int pin_skewer_score(const int value_of_pinner, const int value_of_pinned, const int value_of_pinned_to) {
    int value_putatively_lost = min(value_of_pinned, value_of_pinned_to) - value_of_pinner;
    return max(value_putatively_lost, 1);
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
    return 1 + (piece_value(for_this) - piece_value(trade_this));
}

constexpr int retreat_score(Piece p) {
    return is_major_piece(p) ? 5 : 3;
}

constexpr int desperado_score(int piece_val) {
    return piece_val;
}

constexpr int discovered_score(Piece attacked) {
    if (type(attacked) == QUEEN) {
        return 5;
    } else {
        return 3;
    }
}

constexpr int approach_kings_score(GamePhase phase) {
    return (phase == ENDGAME) ? 2 : 0;
}

constexpr int avoid_mate_score() {
    return 9;
}

#endif //STASE_SCORE_H
