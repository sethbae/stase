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

/*
 * Returns a positive integer representing the value of the piece in millipawns
 */
inline int piece_value(const Piece p) {

    switch (type(p)) {
        case PAWN: return 1000;
        case KNIGHT:
        case BISHOP: return 3000;
        case ROOK: return 5000;
        default: return 9000;
    }

}

int open_line_walk(const Board &, Square s, StepFunc*, MoveType);
int half_line_walk(const Board &, Square, StepFunc*, MoveType);
unsigned count_pawns(const Board &, Square, StepFunc*);

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

Metric king_safety_metric;

#endif //STASE_HEUR_H
