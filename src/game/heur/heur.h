#ifndef STASE_HEUR_H
#define STASE_HEUR_H

#include "game.h"

/*
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
typedef float Metric(const Board &);

/*
 * List of all metrics
 */
extern Metric* const METRICS[];

/*
 * List of all metric names
 */
extern const std::string METRIC_NAMES[];

/*
 * The weights assigned to each metric
 */
extern const int WEIGHTS[];

/*
 * The number of metrics being used
 */
extern const unsigned METRICS_IN_USE;

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

/*
 * Piece activity (also declared in game.h, but repeated here for clarity).
 */
Metric piece_activity_alpha_metric;
Metric piece_activity_beta_metric;
Metric piece_activity_gamma_metric;

/*
 * File and centre control metrics (again, redeclared for clarity).
 */
Metric open_line_control_metric;
Metric centre_control_metric;

/*
 * Metrics related to pawn structure
 */
Metric defended_pawns_metric;
Metric isolated_pawns_metric;
Metric central_pawns_metric;
Metric far_advanced_pawns_metric;

Metric development_metric;

/*
 * King safety metrics
 */
Metric pawns_defend_king_metric;
Metric control_near_king_metric;
Metric king_exposure_metric;

#endif //STASE_HEUR_H
