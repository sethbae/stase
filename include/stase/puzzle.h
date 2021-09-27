#ifndef STASE_PUZZLE_H
#define STASE_PUZZLE_H

bool read_fens(unsigned, std::vector<std::string>&);

bool read_all_fens(std::vector<std::string>&);

std::string random_fen();

struct Gamestate;

void retrieve_all_puzzle_fens(std::vector<std::string> &);
void puzzle_gamestates(std::vector<Gamestate> &);
void puzzle_boards(std::vector<Board> &);

#endif //STASE_PUZZLE_H
