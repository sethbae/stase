#ifndef STASE_PUZZLE_H
#define STASE_PUZZLE_H

/**
 * The puzzle class represents complete puzzles with solutions from the lichess database.
 * The list of moves start with the "setup" move, ie the first move which you have to play
 * is the second move in the list.
 */
class Puzzle {

public:
    std::string fen;
    std::vector<Move> solution_moves;

    Puzzle(const std::string & fen_in, std::vector<Move> soln_in) : fen(fen_in), solution_moves(soln_in) {}

};

bool read_lines(std::vector<std::string> &);
bool read_fens(unsigned, std::vector<std::string> &);

bool read_all_fens(std::vector<std::string> &);

std::string random_fen();

struct Gamestate;

void retrieve_all_puzzle_fens(std::vector<std::string> &);
void puzzle_gamestates(std::vector<Gamestate> &);
void puzzle_boards(std::vector<Board> &);
void retrieve_all_puzzles(std::vector<Puzzle> &);

#endif //STASE_PUZZLE_H
