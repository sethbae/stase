#ifndef GAME_H
#define GAME_H

#include "../board/board.h"

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

// candidate moves
std::vector<Move> cands(const Gamestate &);

float piece_activity_alpha(const Board &);
float piece_activity_beta(const Board &);
float piece_activity_gamma(const Board &);

float centre_control(const Board &);

// control functions
unsigned alpha_control(const Board &, const Square);
unsigned beta_control(const Board &, const Square);
unsigned gamma_control(const Board &, const Square);

int control_count(const Board &, const Square);
void display_control_counts(const Board &);

#endif
