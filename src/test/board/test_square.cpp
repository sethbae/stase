#include "../test.h"
#include "board.h"

struct SetGetTestCase {
    const std::string_view fen;
    const Square sq;
    const Piece p;
};

bool evaluate_set_get_test_case(const SetGetTestCase * tc) {
    Board b(fen_to_board(tc->fen));
    b.set(tc->sq, tc->p);
    return b.get(tc->sq) == tc->p;
}

bool test_set_get_square() {

    const int N = 1000;
    const std::vector<Piece> pieces{
        B_KING, B_QUEEN, B_ROOK, B_KNIGHT, B_BISHOP, B_PAWN,
        W_KING, W_QUEEN, W_ROOK, W_KNIGHT, W_BISHOP, W_PAWN
    };

    std::vector<SetGetTestCase> cases;
    cases.reserve(N);

    for (int i = 0; i < N; ++i) {
        Piece p = pieces[rand() % pieces.size()];
        int x = rand() % 8;
        int y = rand() % 8;
        Square s{(Byte) x, (Byte) y};
        cases.push_back(SetGetTestCase{starting_fen(), s, p});
    }

    TestSet<SetGetTestCase> test_set{
        "board-set-get-square",
        cases
    };

    return evaluate_test_set(&test_set, & evaluate_set_get_test_case);
}
