#include <iostream>
#include "puzzle.h"
#include "game.h"
#include "../utils/utils.h"

bool score_puzzle_against_cands(const Puzzle & puzzle) {

    Gamestate gs(puzzle.fen);

    for (int i = 0; i < puzzle.solution_moves.size(); ++i) {

        // the solver only has to find the odd moves
        if (i % 2 == 1) {

            std::vector<Move> candidates = cands(gs)->critical;

            bool found = false;
            for (const Move & cand_move: candidates) {
                if (equal(cand_move, puzzle.solution_moves[i])) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                return false;
            }
        }

        gs.next_in_place(puzzle.solution_moves[i]);

    }

    return true;

}

void test_cands_against_puzzles() {

    std::cout << "Loading dataset...\n";

    std::vector<Puzzle> puzzles;
    retrieve_all_puzzles(puzzles);

    std::cout << "Evaluating cands performance\n";
    std::cout << "|                   |\n";

    int succ = 0;

    for (int i = 0; i < puzzles.size(); ++i) {
        if ((i % (puzzles.size() / 20)) == 0) {
            std::cout << "*";
            std::cout.flush();
        }
        if (score_puzzle_against_cands(puzzles[i])) {
            ++succ;
        }
    }

    double percent = ((double) succ * 100) / ((double) puzzles.size());

    std::cout << "\nThe candidates function highlighted correct moves for:\n\n";
    std::cout << succ << "/" << puzzles.size() << "  =  " << dp_2(percent) << "%\n\n";
    std::cout << "Of puzzles in the lichess database.\n";

}
