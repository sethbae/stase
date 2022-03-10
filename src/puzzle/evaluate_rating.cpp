#include <iostream>
#include "puzzle.h"
#include "game.h"
#include "../utils/utils.h"
#include "../game/gamestate.hpp"
#include "search.h"
#include "../search/search_tools.h"
#include "../search/thread.h"

bool score_puzzle_against_cands(const Puzzle & puzzle) {

    Gamestate gs = *fresh_gamestate(puzzle.fen);

    for (int i = 0; i < puzzle.solution_moves.size(); ++i) {

        // the solver only has to find the odd moves
        if (i % 2 == 1) {

            std::vector<Move> candidates = cands(gs, new CandSet)->critical;

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

bool score_engine_against_puzzle(const Puzzle & puzzle, int positions_allowed) {

    // the puzzles start with your opponent playing a move, so simulate this
    Gamestate gs(puzzle.fen);
    gs.next_in_place(puzzle.solution_moves[0]);

    // then run the engine
    run_with_node_limit(board_to_fen(gs.board), positions_allowed);

    // get the UCI moves it played
    std::vector<SearchNode *> line_played = retrieve_trust_line(fetch_root());

    pr_board_conf(fen_to_board(puzzle.fen));
    std::cout << "Rating: " << puzzle.rating << "\n";
    for (int i = 0; i < puzzle.solution_moves.size(); ++i) {
        std::cout << move2uci(puzzle.solution_moves[i]) << " ";
    }
    std::cout << "\n";
    for (int i = 1; i < line_played.size(); ++i) {
        std::cout << move2uci(line_played[i]->move) << " ";
    }
    std::cout << "\n";

    if (line_played.size() < puzzle.solution_moves.size()) {
        return false;
    }

    for (int i = 1; i < puzzle.solution_moves.size(); ++i) {
        if (!equal(puzzle.solution_moves[i], line_played[i + 1]->move)) {
            return false;
        }
    }
    return true;
}

void test_engine_against_puzzles(int num_puzzles, int positions_per_puzzle, float rating_cap) {

    std::cout << "Loading dataset...\n";

    std::vector<Puzzle> puzzles;
    retrieve_all_puzzles(puzzles);

    std::cout << "Evaluating engine performance\n";
    std::cout << "|                   |\n";

    int succ = 0;
    int attempted = 0;
    for (int i = 0; attempted < num_puzzles && i < puzzles.size(); ++i) {
        if (puzzles[i].rating > rating_cap) {
            continue;
        }
        if ((i % (puzzles.size() / 20)) == 0) {
            std::cout << "*";
            std::cout.flush();
        }
        if (score_engine_against_puzzle(puzzles[i], positions_per_puzzle)) {
            ++succ;
        }
        ++attempted;
    }

    double percent = ((double) succ * 100) / ((double) attempted);

    std::cout << "\nThe candidates function highlighted correct moves for:\n\n";
    std::cout << succ << "/" << num_puzzles << "  =  " << dp_2(percent) << "%\n\n";
    std::cout << "Of the first " << num_puzzles << " puzzles in the lichess database.\n";
}
