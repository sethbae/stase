#include <vector>
#include <string>
#include <iostream>

#include "board.h"
#include "game.h"
#include "puzzle.h"
#include "../utils/utils.h"
#include "../game/gamestate.hpp"

/**
 * Reads and returns every FEN from the lichess puzzle database. This uses caching - it's cheap!
 * @param vec the vector to fill.
 */
void retrieve_all_puzzle_fens(std::vector<std::string> & vec) {

    static std::vector<std::string> cache;

    if (cache.empty()) {
        read_all_fens(cache);
    }

    vec = cache;

}

/**
 * Reads and returns every FEN from the lichess puzzle database, initialised as a gamestate.
 * This uses caching - it's cheap!
 * @param vec the vector to fill.
 */
void puzzle_gamestates(std::vector<Gamestate> & vec) {

    std::vector<Gamestate> cache;

    if (cache.empty()) {
        std::vector<std::string> fens;
        read_all_fens(fens);
        for (int i = 0; i < fens.size(); ++i) {
            cache.push_back(Gamestate(fens[i]));
        }
    }

    vec = cache;

}

/**
 * Reads and returns every FEN from the lichess puzzle database, initialised as a board.
 * This uses caching - it's cheap!
 * @param vec the vector to fill.
 */
void puzzle_boards(std::vector<Board> & vec) {

    static std::vector<Board> cache;

    if (cache.empty()) {
        std::vector<std::string> fens;
        read_all_fens(fens);
        for (std::string fen : fens) {
            cache.push_back(fen_to_board(fen));
        }
    }

    vec = cache;

}

void retrieve_all_puzzles(std::vector<Puzzle> & vec) {

    static std::vector<Puzzle> cache;

    if (cache.empty()) {
        std::vector<std::string> csv_lines;
        csv_lines.reserve(2500000);

        read_lines(csv_lines);

        for (std::string csv_line : csv_lines) {

            // skip first field of csv
            csv_line = csv_line.substr(csv_line.find_first_of(',') + 1);

            int first_comma = csv_line.find(',', 0);
            int second_comma = csv_line.find(',', first_comma + 1);
            int third_comma = csv_line.find(',', second_comma + 1);

            const std::string fen = csv_line.substr(0, first_comma);
            const std::string move_str = csv_line.substr(first_comma + 1, second_comma - first_comma - 1);
            const std::string rating_str = csv_line.substr(second_comma + 1, third_comma - second_comma - 1);

            std::vector<Move> moves;

            int n = 0;
            while (n < move_str.size()) {

                int i;
                for (i = n; i < move_str.size() && move_str[i] != ' '; ++i)
                    ;

                // n ... i is now a substring containing a move description

                // this is bounds-safe iff the moves are all at least 4 chars, which should be the case
                std::string four_char_san = move_str.substr(n, 4);
                moves.push_back(uci2move(four_char_san));
                if (i == n + 5) {
                    // promotion char was included
                    Piece prom_piece = ctop(move_str[n + 5]);
                    moves[moves.size() - 1].set_prom_piece(type(prom_piece));
                }

                n = i + 1;
            }

            cache.emplace_back(fen, moves, std::stod(rating_str));
        }

    }

    vec = cache;

}
