#ifndef STASE_GLOGIC_H
#define STASE_GLOGIC_H

#include "game.h"
#include "board.h"
#include "piece_moves.hpp"
#include "piece_encounter_cache.hpp"
#include "edge_of_board_lookup.h"
#include "../../utils/ptr_vec.h"

/**
 * Used to pass information about the pieces attacking and controlling a certain square.
 */
struct SquareControlStatus {
    int16_t balance = 0;
    uint16_t min_w = 0;
    uint16_t min_b = 0;

    inline void print() {
        std::cout << "Status: b " << balance
                    << ", min_w " << (int)min_w
                    << ", min_b " << (int)min_b << "\n";
    }
    inline void print(const Square s) {
        std::cout << "[" << sqtos(s) << "] ";
        print();
    }
};

// Special values used in min_w and min_b fields to distinguish cases relevant for kings.
const uint16_t ATTACKED_BY_PINNED_PIECE = 11;
const uint16_t NOT_ATTACKED_AT_ALL = 12;

/**
 * Checks whether the three points given all lie on a straight line.
 */
inline bool collinear_points(Square a, Square b, Square c) {
    return (get_y(a) - get_y(b)) * (get_x(b) - get_x(c)) == (get_y(b) - get_y(c)) * (get_x(a) - get_x(b));
}

/**
 * Geometry functions: what squares can a piece move to, can it reach this line etc.
 */
bool can_move_to_square(const Board &, Square from, Square to);
Delta open_path_between(const Board & b, const Square, const Square);
Square square_piece_can_reach_on_line(const Board & b, const Square, const Square, const Square);
std::vector<Square> squares_piece_can_reach_on_line(const Board & b, const Square, const Square, const Square);

float piece_activity_alpha(const Board &);
float piece_activity_beta(const Board &);
float piece_activity_gamma(const Board &);

int w_pawn_defence_count(const Gamestate &, const Square);
int b_pawn_defence_count(const Gamestate &, const Square);
int pawn_defence_count(const Gamestate &, const Square);

/**
 * Control functions: how many pieces control this square, and does a piece cover a given square.
 */
int control_count(const Gamestate &, Square);
int alpha_control(const Board &, Square);
int beta_control(const Board &, Square);
int gamma_control(const Board &, Square);
bool alpha_covers(const Board &, Square piece_sq, Square target_sq);
bool beta_covers(const Board &, Square piece_sq, Square target_sq);
bool gamma_covers(const Board &, Square piece_sq, Square target_sq);

/**
 * Safety functions: is this square weak, and would it be, after a move?
 */
SquareControlStatus evaluate_square_control(const Gamestate &, const Square);
bool is_weak_status(const Gamestate &, const Square, const Colour colour, SquareControlStatus);

bool is_weak_square(const Gamestate &, const Square, const Colour colour, const bool use_caches = true);
bool would_be_weak_after(const Gamestate &, const Square s, const Colour colour, const Move m);

bool has_safe_king(const Gamestate &gs, const Colour king_colour);
bool would_be_safe_king_square(const Gamestate &, const Square, const Colour colour);
bool would_be_safe_for_king_after(const Gamestate &, const Square, const Move, const Colour colour);
bool is_unsafe_piece(const Gamestate &, const Square);
bool would_be_unsafe_after(const Gamestate &, const Square s, const Move m);

bool move_is_safe(const Gamestate &, const Move);
bool zero_or_worse_control(const Gamestate &, const Square);
bool totally_undefended(const Gamestate &, const Colour, const Square);
void find_invalidated_squares(const Board &, const Move, ptr_vec<Square> &);

bool pawn_is_en_passantable(const Gamestate &, const Square);

/**
 * More efficient versions of the board API (eg, for legal moves etc)
 */
void find_pawn_moves_to(const Gamestate &, std::vector<Move> &, const Colour, const Square);
bool can_see_immediately(const Gamestate &, const Piece, const Square, const Square);
bool can_see_x_ray(const Gamestate &, const Piece, const Square, const Square);

bool is_knight_move(const Square, const Square);
bool is_king_move(const Square, const Square);
bool is_pawn_capture(const Colour, const Square, const Square);
bool piece_covers_from_square(const Gamestate &, const Piece, const Square, const Square);

/**
 * Helper functions: printing mostly.
 */
void display_control_counts(const Gamestate &);

#endif //STASE_GLOGIC_H
