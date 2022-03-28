#ifndef STASE_BOARD_H
#define STASE_BOARD_H

#include <string>
#include <vector>

#include "../../src/board/base_types.h"
#include "../../src/board/square.h"
#include "../../src/board/move.h"

/**
 * The board struct represents a fully-specified board position: pieces and their
 * locations, but also whose turn it is etc. Everything required for FIDE chess.
 *
 * There is no validity requirement for the represented position. For example, you
 * can have no kings, be in check on your opponents turn etc.
 */
struct Board;

/** special values: empty, starting */
Board empty_board();
Board starting_pos();
constexpr std::string_view starting_fen() {
    return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

/**
 * Legal moves. These functions provide all legal moves, those only for a certain piece
 * as well as checking whether you're in check or not.
 */

void piecemoves_ignore_check(const Board &, const Square, std::vector<Move> &);
bool in_check_hard(const Board &);
bool in_check_hard(const Board &, Colour);
void legal_moves(const Board &, std::vector<Move> &);
std::vector<Move> legal_moves(const Board &);

/**
 * Reading and writing helpers: into and out of FEN and displaying usefully
 * either to stdout or to a given ostream.
 */

Board fen_to_board(const std::string_view & fen);
std::string board_to_fen(const Board &);
std::vector<std::string> read_pgn(const std::string &s);
// write functions (write to ostream)
void wr_board(const Board &, std::ostream &);
void wr_board(const Board &, const std::string & indent, std::ostream &);
void wr_board_conf(const Board &, std::ostream &);
void wr_board_conf(const Board &, const std::string & indent, std::ostream &);
void wr_bin_64(uint64_t, std::ostream &);
// print functions (write to stdout using cout)
void pr_board(const Board &);
void pr_board(const Board &, const std::string & indent);
void pr_board_conf(const Board &);
void pr_board_conf(const Board &, const std::string & indent);
void pr_bin_64(uint64_t);

#endif //STASE_BOARD_H
