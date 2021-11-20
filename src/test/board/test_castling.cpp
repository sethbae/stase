#include "../test.h"
#include "board.h"

struct CastleTestCase {
    const std::string fen;
    const bool w_short;
    const bool w_long;
    const bool b_short;
    const bool b_long;
};

const TestSet<CastleTestCase> castle_test_set{
        "board-castling-legality",
        {
                CastleTestCase{
                    "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",
                    true,
                    true,
                    true,
                    true
                },
                CastleTestCase{
                    "r3k2r/8/8/8/8/8/8/R3KN1R w KQkq - 0 1",
                    false,
                    true,
                    true,
                    true
                },
                CastleTestCase{
                    "r3k2r/8/8/8/8/8/8/R2bK2R w KQkq - 0 1",
                    true,
                    false,
                    true,
                    true
                },
                CastleTestCase{
                    "r3kN1r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",
                    true,
                    true,
                    false,
                    true
                },
                CastleTestCase{
                    "rq2k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",
                    true,
                    true,
                    true,
                    false
                },
                CastleTestCase{
                    "rq2k2r/b7/8/8/8/8/8/R3K2R w KQkq - 0 1",
                    false,
                    true,
                    true,
                    false
                },
        }
};

bool evaluate_castle_test_case(const CastleTestCase * tc) {

    Board b = fen_to_board(tc->fen);

    // these are the moves representing each of four castles
    Move ws = Move{stosq("e1"), stosq("g1")};
    Move wl = Move{stosq("e1"), stosq("c1")};
    Move bs = Move{stosq("e8"), stosq("g8")};
    Move bl = Move{stosq("e8"), stosq("c8")};

    // create a list which is all the legal moves for either player
    std::vector<Move> legals = legal_moves(b);
    b.flip_white();
    std::vector<Move> legals2 = legal_moves(b);
    legals.insert(legals.end(), legals2.begin(), legals2.end());

    // and now check four booleans, that each one is/isn't legal
    bool w_short = false;
    bool w_long = false;
    bool b_short = false;
    bool b_long = false;

    for (const Move & m: legals) {
        if (equal(m, ws)) {
            w_short = true;
        }
        if (equal(m, wl)) {
            w_long = true;
        }
        if (equal(m, bs)) {
            b_short = true;
        }
        if (equal(m, bl)) {
            b_long = true;
        }
    }

    // and compare it to the expected values in the test case
    return w_short == tc->w_short
           && w_long == tc->w_long
           && b_short == tc->b_short
           && b_long == tc->b_long;
}

bool test_castling() {
    return evaluate_test_set(&castle_test_set, &evaluate_castle_test_case);
}
