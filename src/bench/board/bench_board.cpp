#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;
#include <vector>
using std::vector;
#include <string>
using std::string;

#include "board.h"
#include "puzzle.h"
#include "../bench.h"
#include "../../board/board.hpp"


struct BoardPairParam {
    mutable Board board;
    mutable Board blank_board;
};

int write_board_xy(const BoardPairParam & param) {
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            param.blank_board.set(x, y, param.board.get(x, y));
        }
    }
    return param.board.get_white() ? 1 : 0;
}

int write_board_yx(const BoardPairParam & param) {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            param.blank_board.set(x, y, param.board.get(x, y));
        }
    }
    return param.board.get_white() ? 1 : 0;
}

/**
 * Benchmarks the read/write time for a whole board. It sets every square of a blank board
 * to a value got from a puzzle board. There are three different strategies compared for
 * iterating over the board: see above for the implementations.
 */
void bench_board_write() {

    std::vector<Board> boards;
    puzzle_boards(boards);

    std::vector<BoardPairParam> params;
    for (int i = 0; i < boards.size(); ++i) {
        Board b = empty_board();
        params.push_back(BoardPairParam{boards[i], b});
    }

    bench("board-get-set-xy", NANOS, params.data(), params.size(), &write_board_xy);
    bench("board-get-set-yx", NANOS, params.data(), params.size(), &write_board_yx);

}

int write_config_individually(const BoardPairParam & param) {

    int sum = 0;
    int t;

    param.blank_board.set_white((t = param.board.get_white()));
    sum += t;
    param.blank_board.set_cas_ws((t = param.board.get_cas_ws()));
    sum += t;
    param.blank_board.set_cas_wl((t = param.board.get_cas_wl()));
    sum += t;
    param.blank_board.set_cas_bs((t = param.board.get_cas_bs()));
    sum += t;
    param.blank_board.set_cas_bl((t = param.board.get_cas_bl()));
    sum += t;
    param.blank_board.set_ep_exists((t = param.board.get_ep_exists()));
    sum += t;
    param.blank_board.set_ep_file((t = param.board.get_ep_file()));
    sum += t;
    param.blank_board.set_halfmoves((t = param.board.get_halfmoves()));
    sum += t;
    param.blank_board.set_wholemoves((t = param.board.get_wholemoves()));

    return sum + t;

}

int write_config_word(const BoardPairParam & param) {
    int t;
    param.blank_board.set_conf_word(t = param.board.get_conf_word());
    return t;
}

/**
 * Benchmarks how long it takes to benchmark the reading and writing of the boards config. First it copies
 * (using get then set) each individual component of the config, and the second benchmark simply transfers
 * the entire config word.
 */
void bench_board_write_config() {

    std::vector<Board> boards;
    puzzle_boards(boards);

    std::vector<BoardPairParam> params;
    for (int i = 0; i < boards.size(); ++i) {
        Board b = empty_board();
        params.push_back(BoardPairParam{boards[i], b});
    }

    bench("board-copy-conf-ind", NANOS, params.data(), params.size(), &write_config_individually);
    bench("board-copy-conf-word", NANOS, params.data(), params.size(), &write_config_word);

}

int board_iteration_xy(const Board & b) {
    int sum = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            sum += b.get(x, y);
        }
    }
    return sum;
}
int board_iteration_yx(const Board & b) {
    int sum = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            sum += b.get(x, y);
        }
    }
    return sum;
}

/**
 * Iterates over the board in one of three ways, as above: incrementing x then y, y then x, or straight through 64.
 * It has to call get on each square, to avoid being optimised away, which probably constitutes a significant portion
 * of the time taken.
 */
void bench_board_iteration() {

    std::vector<Board> boards;
    puzzle_boards(boards);

    bench("board-iter-xy", NANOS, boards.data(), boards.size(), &board_iteration_xy);
    bench("board-iter-yx", NANOS, boards.data(), boards.size(), &board_iteration_yx);

}

void bench_in_check() {

    std::vector<Board> boards;
    puzzle_boards(boards);

    bench("board-in-check-hard", MICROS, boards.data(), boards.size(), &in_check_hard);

}

int check_legal_moves(const Board & b) {
    std::vector<Move> moves = legal_moves(b);
    return moves.size();
}

void bench_legal_moves() {

    std::vector<Board> boards;
    puzzle_boards(boards);

    bench("board-legal-moves", MICROS, boards.data(), boards.size(), &check_legal_moves);

}

int bench_board(void) {

    bench_board_write();
    bench_board_write_config();
    bench_board_iteration();
    bench_in_check();
    bench_legal_moves();

    return 0;
}
