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
    // [0]: OPENING [1]: MIDGAME [2]: ENDGAME
    const int weights[3];
    float (*metric)(const Gamestate &);
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
float piece_activity_alpha_metric(const Gamestate &);
float piece_activity_beta_metric(const Gamestate &);
float piece_activity_gamma_metric(const Gamestate &);

// line control
float open_line_control_metric(const Gamestate &);
float centre_control_metric(const Gamestate &);

// pawn structure metrics
float defended_pawns_metric(const Gamestate &);
float isolated_pawns_metric(const Gamestate &);
float central_pawns_metric(const Gamestate &);
float far_advanced_pawns_metric(const Gamestate &);

// opening metrics
float development_metric(const Gamestate &);
float castling_metric(const Gamestate &);
float castling_rights_metric(const Gamestate &);

// king safety metrics
float pawns_defend_king_metric(const Gamestate &);
float control_near_king_metric(const Gamestate &);
float king_exposure_metric(const Gamestate &);

const Metric __piece_activity_alpha_metric{
    "piece-activity-alpha",
    {600, 600, 600 },
    &piece_activity_alpha_metric
};
const Metric __piece_activity_beta_metric{
    "piece-activity-beta",
    {600, 600, 600 },
    &piece_activity_beta_metric
};
const Metric __piece_activity_gamma_metric{
    "piece-activity-gamma",
    {600, 600, 600 },
    &piece_activity_gamma_metric
};
const Metric __open_line_control_metric{
    "open-line-control",
    {800, 1000, 1200 },
    &open_line_control_metric
};
const Metric __centre_control_metric{
    "centre-control",
    {2000, 1500, 1250 },
    &centre_control_metric
};
const Metric __defended_pawns_metric{
    "defended-pawns",
    {400, 500, 750 },
    &defended_pawns_metric
};
const Metric __isolated_pawns_metric{
    "isolated-pawns",
    {300, 400, 600 },
    &isolated_pawns_metric
};
const Metric __central_pawns_metric{
    "central-pawns",
    {600, 500, 400 },
    &central_pawns_metric
};
const Metric __far_advanced_pawns_metric{
    "far-advanced-pawns",
    {100, 400, 1000 },
    &far_advanced_pawns_metric
};
const Metric __development_metric{
    "development",
    {1000, 500, 0 },
    &development_metric
};
const Metric __castling_metric{
    "castling",
    {300, 200, 0 },
    &castling_metric
};
const Metric __castling_rights_metric{
    "castling-rights",
    {400, 300, 0 },
    &castling_rights_metric
};
const Metric __pawns_defend_king_metric{
    "pawns-defend-king",
    {500, 1000, 400 },
    &pawns_defend_king_metric
};
const Metric __control_near_king_metric{
    "control-near-king",
    {500, 1000, 400},
    &control_near_king_metric
};
const Metric __king_exposure_metric{
    "king-exposure",
    {500, 1000, 400 },
    &king_exposure_metric
};

const std::vector<const Metric *> ALL_METRICS{

    &__piece_activity_alpha_metric,
    &__piece_activity_beta_metric,
    &__piece_activity_gamma_metric,
    &__open_line_control_metric,
    &__centre_control_metric,
    &__defended_pawns_metric,
    &__isolated_pawns_metric,
    &__central_pawns_metric,
    &__far_advanced_pawns_metric,
    &__development_metric,
    &__castling_metric,
    &__castling_rights_metric,
    &__pawns_defend_king_metric,
    &__control_near_king_metric,
    &__king_exposure_metric

};

#endif //STASE_HEUR_H
