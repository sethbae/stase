#ifndef STASE_GAME_H
#define STASE_GAME_H

#include "board.h"
#include "../../src/game/cands/cands.h"

enum MoveType {
    ORTHO = 0,
    DIAG = 1,
    KNIGHT_MOVE = 2,
    PAWN_MOVE = 3,
    KING_MOVE = 4,
    INVALID_MOVE = 5
};

/*
 * Arrays for diffs which can be added to a square; e.g. (1,1) to move diagonally up right.
 * Given values in gamestate.cpp
 */
extern const int XD[];
extern const int YD[];
extern const int XKN[];
extern const int YKN[];

// lookup tables for the step function which moves in each direction
typedef void StepFunc(Square &);
extern StepFunc *STEP_FUNCS[8];

// constants for iterating over the directions
const unsigned ORTHO_START = 0;
const unsigned ORTHO_STOP  = 4;
const unsigned DIAG_START  = 4;
const unsigned DIAG_STOP   = 8;

// and a lookup table (indexed by numeric value of Ptype enumeration) for which of
// the above movesets to use.
const bool PIECE_MOVE_TYPES[][6] = {
        { false, false, false, false, true, false },    // KING
        { true, true, false, false, false, false },     // QUEEN
        { true, false, false, false, false, false },    // ROOK
        { false, false, true, false, false, false },    // KNIGHT
        { false, true, false, false, false, false },    // BISHOP
        { false, false, false, true, false, false },    // PAWN
        { false, false, false, false, false, false },   // ---------invalid-----------
        { false, false, false, false, false, false },   // ---------invalid-----------
        { false, false, false, false, true, false },    // KING
        { true, true, false, false, false, false },     // QUEEN
        { true, false, false, false, false, false },    // ROOK
        { false, false, true, false, false, false },    // KNIGHT
        { false, true, false, false, false, false },    // BISHOP
        { false, false, false, true, false, false },    // PAWN
        { false, false, false, false, false, false }    // ---------invalid-----------
};

// returns true if the given piece can move in the given way, false otherwise
inline bool can_move(Piece piece, MoveType dir) {
    return PIECE_MOVE_TYPES[piece][dir];
}

struct Gamestate {

    // The enclosed board
    Board board;

    FeatureFrame** features;

    // Attack and occupation maps for each of the adversaries
    Bitmap fattack;
    Bitmap eattack;
    Bitmap foccupy;
    Bitmap eoccupy;

    // Overall vacancy and occupation maps
    Bitmap vacancy;
    Bitmap occupancy;

    // Locations of all pieces of a certain type (can be ANDed with occupation to isolate pieces of desired colour)
    Bitmap kings;
    Bitmap pawns;
    Bitmap queens;
    Bitmap knights;
    Bitmap bishops;
    Bitmap rooks;

    Move last_move;
    Piece last_capture;

    Gamestate();
    Gamestate(const Board &);
    void recalculate_all();
    void recalculate_attacks();
    void recalculate_positions();
    void update(Move m);

};

bool in_check(Gamestate &);


// evaluation type and helper/conversion methods
typedef int16_t Eval;

Eval zero();

Eval mate_in(Ptype colour, unsigned);
Eval white_mates_in(unsigned);
Eval black_mates_in(unsigned);

Eval white_has_been_mated();
Eval black_has_been_mated();

float human_eval(int);
int int_eval(Eval);
Eval eval_from_float(float);

bool is_mate(Eval);
bool white_is_mated(Eval);
bool black_is_mated(Eval);

int signed_mate_distance(Eval);
int abs_mate_distance(Eval);

std::string etos(Eval, int digits);
std::string etos(Eval);

// heuristic evaluation
int heur(const Gamestate &);
int heur_with_description(const Gamestate &);

// candidate moves
std::vector<Move> cands(const Gamestate &);

float piece_activity_alpha(const Board &);
float piece_activity_beta(const Board &);
float piece_activity_gamma(const Board &);

// control functions
int alpha_control(const Board &, Square);
int beta_control(const Board &, Square);
int gamma_control(const Board &, Square);

int control_count(const Board &, Square);
void display_control_counts(const Board &);

#endif //STASE_GAME_H
