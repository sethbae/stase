#ifndef STASE_GAME_H
#define STASE_GAME_H

#include "board.h"

enum MoveType {
    ORTHO = 0,
    DIAG = 1,
    KNIGHT_MOVE = 2,
    PAWN_MOVE = 3,
    KING_MOVE = 4,
    INVALID_MOVE = 5
};

struct DeltaPair {
    Byte xd;
    Byte yd;
};
const DeltaPair INVALID_DELTA = { SQUARE_SENTINEL, SQUARE_SENTINEL };
inline bool is_valid_delta(const DeltaPair d) { return d.xd != SQUARE_SENTINEL && d.yd != SQUARE_SENTINEL; }
inline MoveType direction_of_delta(const DeltaPair d) {
    if (d.xd == 0 || d.yd == 0) {
        return ORTHO;
    }
    return DIAG;
}
DeltaPair get_delta_between(const Square, const Square);

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
const unsigned DIAG_START  = 0;
const unsigned DIAG_STOP   = 4;
const unsigned ORTHO_START = 4;
const unsigned ORTHO_STOP  = 8;

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
inline bool can_move_in_direction(Piece piece, MoveType dir) {
    return PIECE_MOVE_TYPES[piece][dir];
}
bool can_move_to_square(const Board &, Square from, Square to);
bool collinear_points(Square, Square, Square);
DeltaPair open_path_between(const Board & b, const Square, const Square);
Square can_move_onto_line(const Board & b, const Square, const Square, const Square);
Square first_piece_encountered(const Board &, const Square, const DeltaPair);

// forward declaration: see cands.h
struct FeatureFrame;

struct Gamestate {

    // The enclosed board
    Board board;

    FeatureFrame ** feature_frames;

    Gamestate();
    Gamestate(const Board &);
    Gamestate(const Gamestate &);
    Gamestate(Gamestate &&);
    ~Gamestate();

    Gamestate & operator=(const Gamestate &) = default;
    Gamestate & operator=(Gamestate &&) = default;

    void recalculate_all();
    void recalculate_attacks();
    void recalculate_positions();
    void update(Move m);
    void repopulate_caches();

    // TODO (GM-25): as and when required add these to the copy constructor

    // store a cache of squares on which pieces fall
    Square * wpieces;
    Square * bpieces;
    inline Square * friendly_pieces() const {
        return board.get_white() ? wpieces : bpieces;
    }
    inline Square * enemy_pieces() const {
        return board.get_white() ? bpieces : wpieces;
    }

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

};

bool in_check(Gamestate &);


// evaluation type and helper/conversion methods
typedef int16_t Eval;

Eval zero();

inline Eval invalid_eval() {
    return (Eval) 0x0F0F;
}

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
std::vector<Move> cands_report(const Gamestate &);

float piece_activity_alpha(const Board &);
float piece_activity_beta(const Board &);
float piece_activity_gamma(const Board &);

// control functions
int alpha_control(const Board &, Square);
int beta_control(const Board &, Square);
int gamma_control(const Board &, Square);

// cover functions
bool alpha_covers(const Board &, Square piece_sq, Square target_sq);
bool beta_covers(const Board &, Square piece_sq, Square target_sq);
bool gamma_covers(const Board &, Square piece_sq, Square target_sq);

bool is_safe_king(const Gamestate &, const Ptype colour);
bool would_be_safe_king_square(const Gamestate &, const Square, const Ptype colour);
bool would_be_safe_for_king_after(const Gamestate &, const Square, const Move, const Ptype colour);

int control_count(const Board &, Square);
void display_control_counts(const Board &);

/**
 * Used to pass information about the pieces attacking and controlling a certain square.
 */
struct SquareControlStatus {
    int balance = 0;
    int min_w = 0;
    int min_b = 0;
};
SquareControlStatus evaluate_square_status(const Gamestate &, const Square);
bool is_weak_status(const Gamestate &, const Square, const Ptype colour, SquareControlStatus);

// weak squares/unsafe pieces
bool is_weak_square(const Gamestate &, const Square, const Ptype colour);
bool would_be_weak_after(const Gamestate &gs, const Square s, const Ptype colour, const Move m);
bool is_unsafe_piece(const Gamestate &, const Square);
bool would_be_unsafe_after(const Gamestate &gs, const Square s, const Move m);

#endif //STASE_GAME_H
