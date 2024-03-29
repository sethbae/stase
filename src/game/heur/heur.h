#ifndef STASE_HEUR_H
#define STASE_HEUR_H

#include "../../../include/stase/game.h"

/**
 * Metrics measure concretely some element of the board and score it as in favour of white
 * or black. They should not anticipate pieces moving: tactics should play out elsewhere.
 * Thus it is the (somewhat unreliable) assumption that here, we have a quiescent position,
 * and that the balance can therefore be assessed by things like pawn structure, piece
 * activity, etc.
 *
 * Dynamic factors can still be accounted for of course; such as king safety or far
 * advanced pawns (this is different to accounting for the literal mating combination
 * or the sequence of concrete moves by which a pawn promotes).
 *
 * Each metric maps the board to a float. They return a positive (resp. negative)
 * number -1 <= x <= 1 indicating favour to one side or another.
 */
struct Metric {
    const std::string name;
    const int id;
    float (*metric)(const Gamestate &);
};

namespace __piece_constants{
    const int PAWN_VAL = 1000;
    const int BISHOP_VAL = 3000;
    const int KNIGHT_VAL = 3000;
    const int ROOK_VAL = 5000;
    const int QUEEN_VAL = 9000;
    const int KING_VAL = 9001;
    const uint8_t PAWN_VAL_SIMPLE = 1;
    const uint8_t BISHOP_VAL_SIMPLE = 3;
    const uint8_t KNIGHT_VAL_SIMPLE = 3;
    const uint8_t ROOK_VAL_SIMPLE = 5;
    const uint8_t QUEEN_VAL_SIMPLE = 9;
    const uint8_t KING_VAL_SIMPLE = 10;
}

/**
 * Returns a positive integer representing the value of the given piece in millipawns.
 */
constexpr int piece_value_millis(const Piece p) {
    switch (p) {
        case W_PAWN: return __piece_constants::PAWN_VAL;
        case W_KNIGHT: return __piece_constants::KNIGHT_VAL;
        case W_BISHOP: return __piece_constants::BISHOP_VAL;
        case W_ROOK: return __piece_constants::ROOK_VAL;
        case W_QUEEN: return __piece_constants::QUEEN_VAL;
        case B_PAWN: return __piece_constants::PAWN_VAL;
        case B_KNIGHT: return __piece_constants::KNIGHT_VAL;
        case B_BISHOP: return __piece_constants::BISHOP_VAL;
        case B_ROOK: return __piece_constants::ROOK_VAL;
        case B_QUEEN: return __piece_constants::QUEEN_VAL;
        default: return __piece_constants::KING_VAL;
    }
}

/**
 * Returns an integer between 0 and 10 representing the traditional value of the piece
 * (10 for kings).
 */
constexpr uint8_t piece_value(const Ptype p) {
    switch (p) {
        case PAWN: return __piece_constants::PAWN_VAL_SIMPLE;
        case KNIGHT: return __piece_constants::KNIGHT_VAL_SIMPLE;
        case BISHOP: return __piece_constants::BISHOP_VAL_SIMPLE;
        case ROOK: return __piece_constants::ROOK_VAL_SIMPLE;
        case QUEEN: return __piece_constants::QUEEN_VAL_SIMPLE;
        default: return __piece_constants::KING_VAL_SIMPLE;
    }
}

/**
 * Returns an integer between 0 and 10 representing the traditional value of the piece
 * (10 for kings).
 */
constexpr uint8_t piece_value(const Piece p) {
    switch (p) {
        case W_PAWN: return __piece_constants::PAWN_VAL_SIMPLE;
        case W_KNIGHT: return __piece_constants::KNIGHT_VAL_SIMPLE;
        case W_BISHOP: return __piece_constants::BISHOP_VAL_SIMPLE;
        case W_ROOK: return __piece_constants::ROOK_VAL_SIMPLE;
        case W_QUEEN: return __piece_constants::QUEEN_VAL_SIMPLE;
        case B_PAWN: return __piece_constants::PAWN_VAL_SIMPLE;
        case B_KNIGHT: return __piece_constants::KNIGHT_VAL_SIMPLE;
        case B_BISHOP: return __piece_constants::BISHOP_VAL_SIMPLE;
        case B_ROOK: return __piece_constants::ROOK_VAL_SIMPLE;
        case B_QUEEN: return __piece_constants::QUEEN_VAL_SIMPLE;
        default: return __piece_constants::KING_VAL_SIMPLE;
    }
}

namespace __metrics {

    float __piece_activity_alpha_metric(const Gamestate &);
    float __piece_activity_beta_metric(const Gamestate &);
    float __piece_activity_gamma_metric(const Gamestate &);
    float __open_line_control_metric(const Gamestate &);
    float __centre_control_metric(const Gamestate &);
    float __defended_pawns_metric(const Gamestate &);
    float __isolated_pawns_metric(const Gamestate &);
    float __central_pawns_metric(const Gamestate &);
    float __far_advanced_pawns_metric(const Gamestate &);
    float __development_metric(const Gamestate &);
    float __castling_metric(const Gamestate &);
    float __castling_rights_metric(const Gamestate &);
    float __pawns_defend_king_metric(const Gamestate &);
    float __control_near_king_metric(const Gamestate &);
    float __king_exposure_metric(const Gamestate &);
    float __unconventional_king_metric(const Gamestate &);
    float __promotion_squares(const Gamestate &);
    float __mating_net(const Gamestate &);

    const Metric piece_activity_alpha_metric{
        "piece-activity-alpha",
        0,
        &__piece_activity_alpha_metric
    };
    const Metric piece_activity_beta_metric{
        "piece-activity-beta",
        1,
        &__piece_activity_beta_metric
    };
    const Metric piece_activity_gamma_metric{
        "piece-activity-gamma",
        2,
        &__piece_activity_gamma_metric
    };
    const Metric open_line_control_metric{
        "open-line-control",
        3,
        &__open_line_control_metric
    };
    const Metric centre_control_metric{
        "centre-control",
        4,
        &__centre_control_metric
    };
    const Metric defended_pawns_metric{
        "defended-pawns",
        5,
        &__defended_pawns_metric
    };
    const Metric isolated_pawns_metric{
        "isolated-pawns",
        6,
        &__isolated_pawns_metric
    };
    const Metric central_pawns_metric{
        "central-pawns",
        7,
        &__central_pawns_metric
    };
    const Metric far_advanced_pawns_metric{
        "far-advanced-pawns",
        8,
        &__far_advanced_pawns_metric
    };
    const Metric development_metric{
        "development",
        9,
        &__development_metric
    };
    const Metric castling_metric{
        "castling",
        10,
        &__castling_metric
    };
    const Metric castling_rights_metric{
        "castling-rights",
        11,
        &__castling_rights_metric
    };
    const Metric pawns_defend_king_metric{
        "pawns-defend-king",
        12,
        &__pawns_defend_king_metric
    };
    const Metric control_near_king_metric{
        "control-near-king",
        13,
        &__control_near_king_metric
    };
    const Metric king_exposure_metric{
        "king-exposure",
        14,
        &__king_exposure_metric
    };
    const Metric unconventional_king_metric{
        "unconventional-king",
        15,
        &__unconventional_king_metric
    };
    const Metric promotion_squares_metric{
        "promotion-squares",
        16,
        &__promotion_squares
    };
    const Metric mating_net_metric{
        "mating-net",
        17,
        &__mating_net
    };
}

const std::vector<const Metric *> ALL_METRICS{

    &__metrics::piece_activity_alpha_metric,
    &__metrics::piece_activity_beta_metric,
    &__metrics::piece_activity_gamma_metric,
    &__metrics::open_line_control_metric,
    &__metrics::centre_control_metric,
    &__metrics::defended_pawns_metric,
    &__metrics::isolated_pawns_metric,
    &__metrics::central_pawns_metric,
    &__metrics::far_advanced_pawns_metric,
    &__metrics::development_metric,
    &__metrics::castling_metric,
    &__metrics::castling_rights_metric,
    &__metrics::pawns_defend_king_metric,
    &__metrics::control_near_king_metric,
    &__metrics::king_exposure_metric,
    &__metrics::unconventional_king_metric,
    &__metrics::promotion_squares_metric,
    &__metrics::mating_net_metric

};

#endif //STASE_HEUR_H
