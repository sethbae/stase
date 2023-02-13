#ifndef STASE_BOARD_H
#define STASE_BOARD_H

#include <string>
#include <vector>

#include "../../src/board/base_types.h"
#include "../../src/board/square.h"
#include "../../src/board/move.h"
#include "../../src/utils/ptr_vec.h"

/**
 * The board struct represents a fully-specified board position: pieces and their
 * locations, but also whose turn it is etc. Everything required for FIDE chess.
 *
 * There is no validity requirement for the represented position. For example, you
 * can have no kings, be in check on your opponents turn etc.
 */
struct Board;

inline std::string starting_fen() {
    return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

/**
 * Legal moves. These functions provide all legal moves, those only for a certain piece
 * as well as checking whether you're in check or not.
 */

void piecemoves_ignore_check(const Board &, Square, ptr_vec<Move> &);
bool in_check_hard(const Board &);
bool in_check_hard(const Board &, Colour);
void legal_moves(const Board &, std::vector<Move> &);
std::vector<Move> legal_moves(const Board &);

/**
 * Reading and writing helpers: into and out of FEN and displaying usefully.
 */

namespace board_utils {
    Board fen_to_board(const std::string_view & fen);
    std::string board_to_fen(const Board &);
    void wr_board_conf(const Board &, std::ostream &);
    void print(const Board &);
    void pr_board_conf(const Board &);
}

#endif //STASE_BOARD_H
