#ifndef STASE_HEUR_UTIL_H
#define STASE_HEUR_UTIL_H

#include "heur.h"

/**
 * Useful collections of squares
 */
extern const Square INNER_CENTRAL_SQUARES[];
extern const unsigned NUM_INNER_CENTRAL_SQUARES;
extern const Square OUTER_CENTRAL_SQUARES[];
extern const unsigned NUM_OUTER_CENTRAL_SQUARES;

int open_line_walk(const Board &, Square s, Delta, MoveType);
int half_line_walk(const Board &, Square, Delta, MoveType);
int count_pawns(const Board &, Square, Delta);

#endif //STASE_HEUR_UTIL_H
