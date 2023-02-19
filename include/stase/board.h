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

enum GameStatus {
    ONGOING,
    WHITE_WON,
    BLACK_WON,
    STALEMATE,
    DRAW_BY_THREEFOLD,
    DRAW_BY_FIFTY_MOVES
};

inline std::string starting_fen() {
    return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

/**
 * Legal moves. These functions provide all legal moves, those only for a certain piece
 * as well as checking whether you're in check or not.
 */
namespace game_rules {

    /** @return true if the player to move is in check */
    bool in_check(const Board &);

    /** @return true if the player of the given colour is in check */
    bool in_check(const Board &, Colour);

    /**
     * Fill the given ptr_vec with the moves which could be made by the piece on the given square,
     * disregarding whether or not they would be moving into check.
     */
    void piecemoves_ignore_check(const Board &, Square, ptr_vec<Move> &);

    /**
     * Fill the given vector with all legal moves available in the given position.
     */
    void legal_moves(const Board &, std::vector<Move> &);

    /**
     * Return a vector containing all legal moves available in the given position.
     */
    std::vector<Move> legal_moves(const Board &);

    /**
     * @return the GameStatus implied by the current position. This is not able to account for
     * threefold repetition.
     */
    GameStatus check_status(const Board &);
    GameStatus check_status(const std::string & fen);

    /**
     * @return the GameStatus implied by the current position, given the preceding positions.
     */
    GameStatus check_status(const std::vector<Board> &);
    GameStatus check_status(const std::vector<std::string> & fens);
}

/**
 * Reading and writing helpers: into and out of FEN and displaying usefully.
 */
namespace board_utils {
    Board fen_to_board(const std::string_view & fen);
    std::string board_to_fen(const Board &);
    void write_conf(const Board &, std::ostream &);
    void print(const Board &);
    void print_conf(const Board &);
}

#endif //STASE_BOARD_H
