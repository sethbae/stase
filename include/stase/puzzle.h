#ifndef STASE_PUZZLE_H
#define STASE_PUZZLE_H

#include <string>
#include <vector>
#include "board.h"

/**
 * The puzzle class represents complete puzzles with solutions from the lichess database.
 * The list of moves start with the "setup" move, ie the first move which you have to play
 * is the second move in the list.
 */
class Puzzle {

public:
    std::string fen;
    std::vector<Move> solution_moves;
    double rating;

    Puzzle(const std::string & fen_in, std::vector<Move> soln_in, double rating_in)
        : fen(fen_in), solution_moves(soln_in), rating(rating_in) {}

};

bool read_lines(std::vector<std::string> &);
bool read_fens(unsigned, std::vector<std::string> &, bool random=true);

bool read_all_fens(std::vector<std::string> &);

struct Gamestate;

void puzzle_gamestates(std::vector<Gamestate> &);
void puzzle_boards(std::vector<Board> &);
void retrieve_all_puzzles(std::vector<Puzzle> &);

bool score_puzzle_against_cands(const Puzzle &);
void test_cands_against_puzzles();
void test_engine_against_puzzles(int num_puzzles, int positions_per_puzzle, float rating_cap);

#endif //STASE_PUZZLE_H
