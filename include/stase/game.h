#ifndef STASE_GAME_H
#define STASE_GAME_H

#include "board.h"
#include "../../src/game/eval.hpp"

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
Delta open_path_between(const Board & b, const Square, const Square);
Square square_piece_can_reach_on_line(const Board & b, const Square, const Square, const Square);
std::vector<Square> squares_piece_can_reach_on_line(const Board & b, const Square, const Square, const Square);
Square first_piece_encountered(const Board &, const Square, const Delta);

// forward declaration: see cands.h
struct FeatureFrame;

struct Gamestate {

    // The enclosed board
    Board board;
    bool has_been_mated = false;

    FeatureFrame ** feature_frames;

    explicit Gamestate();
    explicit Gamestate(const Board &);
    explicit Gamestate(const Gamestate &);
    explicit Gamestate(Gamestate &&);
    ~Gamestate();

    Gamestate & operator=(const Gamestate &) = default;
    Gamestate & operator=(Gamestate &&) = default;

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

    Move last_move;
    Piece last_capture;

};

// heuristic evaluation
Eval heur(const Gamestate &);
Eval heur_with_description(const Gamestate &);

// candidate moves
std::vector<Move> cands(Gamestate &);
std::vector<Move> cands_report(Gamestate &);

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

bool is_safe_king(const Gamestate &, const Colour colour);
bool would_be_safe_king_square(const Gamestate &, const Square, const Colour colour);
bool would_be_safe_for_king_after(const Gamestate &, const Square, const Move, const Colour colour);

int control_count(const Board &, Square);
void display_control_counts(const Board &);

int w_pawn_defence_count(const Gamestate &, const Square);
int b_pawn_defence_count(const Gamestate &, const Square);
int pawn_defence_count(const Gamestate &, const Square);

/**
 * Used to pass information about the pieces attacking and controlling a certain square.
 */
struct SquareControlStatus {
    int balance = 0;
    int min_w = 0;
    int min_b = 0;
};
SquareControlStatus evaluate_square_status(const Gamestate &, const Square);
bool is_weak_status(const Gamestate &, const Square, const Colour colour, SquareControlStatus);

// weak squares/unsafe pieces
bool is_weak_square(const Gamestate &, const Square, const Colour colour);
bool would_be_weak_after(const Gamestate &gs, const Square s, const Colour colour, const Move m);
bool is_unsafe_piece(const Gamestate &, const Square);
bool would_be_unsafe_after(const Gamestate &gs, const Square s, const Move m);

#endif //STASE_GAME_H
