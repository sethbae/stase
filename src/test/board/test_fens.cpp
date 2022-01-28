#include "../test.h"
#include "board.h"
#include "puzzle.h"
#include "../../board/board.hpp"

struct FenTestCase {
    const std::string & fen;
};

bool evaluate_fen_test_case(const FenTestCase * tc) {
    const std::string new_fen = board_to_fen(fen_to_board(tc->fen));
    return new_fen.compare(tc->fen) == 0;
}

bool test_read_write_fens() {

    const int N = 1000;

    std::vector<std::string> fens;
    fens.reserve(N);
    read_fens(N, fens);

    std::vector<FenTestCase> cases;
    cases.reserve(N);

    for (int i = 0; i < N; ++i) {
        cases.push_back(FenTestCase{fens[i]});
    }

    TestSet<FenTestCase> test_set{
        "board-read-write-fen",
        cases
    };

    return evaluate_test_set(&test_set, & evaluate_fen_test_case);
}
