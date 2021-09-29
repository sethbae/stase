#include <vector>
#include <string>

#include "board.h"
#include "game.h"
#include "puzzle.h"

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

    static std::vector<Gamestate> cache;

    if (cache.empty()) {
        std::vector<std::string> fens;
        read_all_fens(fens);
        for (std::string & fen : fens) {
            Board b = fen_to_board(fen);
            cache.emplace_back(b);
        }
    }

    // TODO may need to reinstate this...
    // vec = cache;

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
