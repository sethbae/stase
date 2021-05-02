#ifndef GAME_H
#define GAME_H

#include "../board/board.h"

enum MoveType {
    ORTHO = 0,
    DIAG = 1,
    KNIGHT_MOVE = 2,
    PAWN_MOVE = 3,
    KING_MOVE = 4,
    INVALID_MOVE = 5
};

// lookup tables for the step function which moves in each direction
typedef void StepFunc(Square &);
extern const StepFunc *STEP_FUNCS[8];

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

struct Gamestate
{

    // The enclosed board
    Board board;

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

float human_eval(const Eval);
int int_eval(const Eval);
Eval eval_from_float(float);

bool is_mate(const Eval);
bool white_is_mated(const Eval);
bool black_is_mated(const Eval);

int signed_mate_distance(const Eval);
int abs_mate_distance(const Eval);

std::string etos(const Eval, unsigned digits);
std::string etos(const Eval);

// heuristic evaluation
Eval heur(const Gamestate &);
Eval heur_with_description(const Gamestate &);

// candidate moves
std::vector<Move> cands(const Gamestate &);

float piece_activity_alpha(const Board &);
float piece_activity_beta(const Board &);
float piece_activity_gamma(const Board &);

float centre_control(const Board &);
float open_line_control(const Board &);

// control functions
unsigned alpha_control(const Board &, const Square);
unsigned beta_control(const Board &, const Square);
unsigned gamma_control(const Board &, const Square);

int control_count(const Board &, const Square);
void display_control_counts(const Board &);

#endif
