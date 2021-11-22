#ifndef STASE_HEUR_H
#define STASE_HEUR_H

#include "game.h"

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
    const int weight;
    float (*metric)(const Board &);
};

/*
 * Useful collections of squares
 */
extern const Square CENTRAL_SQUARES[];
extern const unsigned NUM_CENTRAL_SQUARES;
extern const Square INNER_CENTRAL_SQUARES[];
extern const unsigned NUM_INNER_CENTRAL_SQUARES;
extern const Square OUTER_CENTRAL_SQUARES[];
extern const unsigned NUM_OUTER_CENTRAL_SQUARES;

const int PAWN_VAL = 1000;
const int BISHOP_VAL = 3000;
const int KNIGHT_VAL = 3000;
const int ROOK_VAL = 5000;
const int QUEEN_VAL = 9000;
const int KING_VAL = 9001;

/**
 * Returns a positive integer representing the value of the given piece type in millipawns.
 */
constexpr int piece_value(const Ptype p) {
    switch (p) {
        case PAWN: return PAWN_VAL;
        case KNIGHT: return KNIGHT_VAL;
        case BISHOP: return BISHOP_VAL;
        case ROOK: return ROOK_VAL;
        case QUEEN: return QUEEN_VAL;
        default: return KING_VAL;
    }
}

/**
 * Returns a positive integer representing the value of the given piece in millipawns.
 */
constexpr int piece_value(const Piece p) {
    switch (p) {
        case W_PAWN: return PAWN_VAL;
        case W_KNIGHT: return KNIGHT_VAL;
        case W_BISHOP: return BISHOP_VAL;
        case W_ROOK: return ROOK_VAL;
        case W_QUEEN: return QUEEN_VAL;
        case B_PAWN: return PAWN_VAL;
        case B_KNIGHT: return KNIGHT_VAL;
        case B_BISHOP: return BISHOP_VAL;
        case B_ROOK: return ROOK_VAL;
        case B_QUEEN: return QUEEN_VAL;
        default: return KING_VAL;
    }
}

int open_line_walk(const Board &, Square s, Delta, MoveType);
int half_line_walk(const Board &, Square, Delta, MoveType);
int count_pawns(const Board &, Square, Delta);

// piece activity
float piece_activity_alpha_metric(const Board &);
float piece_activity_beta_metric(const Board &);
float piece_activity_gamma_metric(const Board &);

// line control
float open_line_control_metric(const Board &);
float centre_control_metric(const Board &);

// pawn structure metrics
float defended_pawns_metric(const Board &);
float isolated_pawns_metric(const Board &);
float central_pawns_metric(const Board &);
float far_advanced_pawns_metric(const Board &);

// opening metrics
float development_metric(const Board &);

// king safety metrics
float pawns_defend_king_metric(const Board &);
float control_near_king_metric(const Board &);
float king_exposure_metric(const Board &);

const std::vector<Metric> ALL_METRICS{
    {
        "piece-activity-alpha",
        600,
        &piece_activity_alpha_metric
    },
    {
        "piece-activity-beta",
        600,
        &piece_activity_beta_metric
    },
    {
        "piece-activity-gamma",
        600,
        &piece_activity_gamma_metric
    },
    {
        "open-line-control",
        1000,
        &open_line_control_metric
    },
    {
        "centre-control",
        1500,
        &centre_control_metric
    },
    {
        "defended-pawns",
        500,
        &defended_pawns_metric
    },
    {
        "development",
        500,
        &development_metric
    },
    {
        "isolated-pawns",
        400,
        &isolated_pawns_metric
    },
    {
        "central-pawns",
        400,
        &central_pawns_metric
    },
    {
        "far-advanced-pawns",
        400,
        &far_advanced_pawns_metric
    },
    {
        "pawns-defend-king",
        750,
        &pawns_defend_king_metric
    },
    {
        "control-near-king",
        750,
        &control_near_king_metric
    },
    {
        "king-exposure",
        750,
        &king_exposure_metric
    },

};

#endif //STASE_HEUR_H
