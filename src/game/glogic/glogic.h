#ifndef STASE_GLOGIC_H
#define STASE_GLOGIC_H

#include "../../../include/stase/game.h"
#include "../../../include/stase/board.h"
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
 * Geometry functions: what squares can a piece move to, can it reach this line etc.
 */
Square square_piece_can_reach_on_line(const Board & b, Square, Square, Square);
void squares_piece_can_reach_on_line(const Board & b, ptr_vec<Square> &, Square, Square, Square);

int w_pawn_defence_count(const Gamestate &, Square);
int b_pawn_defence_count(const Gamestate &, Square);

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
SquareControlStatus evaluate_square_control(const Gamestate &, Square);
SquareControlStatus __sneaked__evaluate_square_control(const Gamestate &, Square);
bool is_weak_status(const Gamestate &, Square, Colour colour, SquareControlStatus);

bool is_weak_square(const Gamestate &, Square, Colour colour, bool use_caches = true);
bool would_be_weak_after(const Gamestate &, Square s, Colour colour, Move m);

bool has_safe_king(const Gamestate &gs, Colour king_colour);
bool would_be_safe_king_square(const Gamestate &, Square, Colour colour);
bool is_unsafe_piece(const Gamestate &, Square);
bool would_be_unsafe_after(const Gamestate &, Square s, Move m);

bool move_is_safe(const Gamestate &, Move);
bool zero_or_worse_control(const Gamestate &, Square);
bool totally_undefended(const Gamestate &, Colour, Square);
void find_invalidated_squares(const Board &, Move, ptr_vec<Square> &);

bool pawn_is_en_passantable(const Gamestate &, Square);

/**
 * More efficient versions of the board API (eg, for legal moves etc)
 */
void find_pawn_moves_to(const Gamestate &, ptr_vec<Move> &, Colour, Square);
bool can_see_immediately(const Gamestate &, Piece, Square, Square);
bool can_see_x_ray(const Gamestate &, Piece, Square, Square);

#endif //STASE_GLOGIC_H
