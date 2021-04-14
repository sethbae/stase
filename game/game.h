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

typedef int_fast16_t Eval;

Eval zero();

Eval mate_in(Ptype colour, unsigned);
Eval white_mates_in(unsigned);
Eval black_mates_in(unsigned);

Eval white_has_been_mated();
Eval black_has_been_mated();

float human_eval(const Eval);
int int_eval(const Eval);

bool is_mate(const Eval);
bool white_is_mated(const Eval);
bool black_is_mated(const Eval);

int signedMateDistance(const Eval);
int absMateDistance(const Eval);

float evalScore(const Eval);


Eval heur(const Gamestate &);

std::vector<Move> cands(const Gamestate &);

#endif
