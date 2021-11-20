#include "../test.h"
#include "board.h"

struct MutationTestCase {
    const std::string start;
    const std::string end;
    const std::string move;
    const bool is_cap = false;
    const bool is_cas = false;
    const bool is_cas_short = false;
    const bool is_ep = false;
    const Ptype prom_piece = KING;
};

TestSet<MutationTestCase> mutation_test_set{
        "board-mutate-hard",
        {
                // queen #1
                MutationTestCase{
                    "8/3q4/8/8/8/3Q4/8/8 w - - 0 1",
                    "8/3q3Q/8/8/8/8/8/8 w - - 0 1",
                    "d3h7"
                },
                // queen #2
                MutationTestCase{
                        "8/3q4/8/8/8/3Q4/8/8 w - - 0 1",
                        "8/3Q4/8/8/8/8/8/8 w - - 0 1",
                        "d3d7",
                        true
                },
                // rook #1
                MutationTestCase{
                        "8/r2Q4/8/8/8/8/8/8 w - - 0 1",
                        "8/3Q4/8/8/8/8/r7/8 w - - 0 1",
                        "a7a2"
                },
                // rook #2
                MutationTestCase{
                        "8/r2Q4/8/8/8/8/8/8 w - - 0 1",
                        "8/3r4/8/8/8/8/8/8 w - - 0 1",
                        "a7d7",
                        true
                },
                // bishop #1
                MutationTestCase{
                        "8/3r4/8/8/6B1/8/8/8 w - - 0 1",
                        "8/3r4/8/8/8/8/4B3/8 w - - 0 1",
                        "g4e2"
                },
                // bishop #2
                MutationTestCase{
                        "8/3r4/8/8/6B1/8/8/8 w - - 0 1",
                        "8/3B4/8/8/8/8/8/8 w - - 0 1",
                        "g4d7",
                        true
                },
                // knight #1
                MutationTestCase{
                        "5n2/3B4/8/8/8/8/8/8 w - - 0 1",
                        "8/3B4/6n1/8/8/8/8/8 w - - 0 1",
                        "f8g6"
                },
                // knight #2
                MutationTestCase{
                        "5n2/3B4/8/8/8/8/8/8 w - - 0 1",
                        "8/3n4/8/8/8/8/8/8 w - - 0 1",
                        "f8d7",
                        true
                },
                // king #1
                MutationTestCase{
                        "8/8/8/q7/8/8/4bn2/4K3 w - - 0 1",
                        "8/8/8/q7/8/8/4Kn2/8 w - - 0 1",
                        "e1e2",
                        true
                },
                // king #2
                MutationTestCase{
                        "8/8/8/q7/8/8/4bn2/4K3 w - - 0 1",
                        "8/8/8/q7/8/8/4bK2/8 w - - 0 1",
                        "e1f2",
                        true
                },
                // king (castle ws) #3
                MutationTestCase{
                        "r3kb1r/pp3ppp/4b1q1/3pQ3/3N4/P1P5/1P3PPP/R1B1K2R w KQkq - 1 15",
                        "r3kb1r/pp3ppp/4b1q1/3pQ3/3N4/P1P5/1P3PPP/R1B2RK1 w KQkq - 1 15",
                        "e1g1",
                        false,
                        true,
                        true
                },
                // king (castle wl) #4
                MutationTestCase{
                        "r1bq1rk1/pp3ppp/2n1p3/2Pp4/3Pn3/2BB1N2/PPQ2PPP/R3K2R w KQ - 4 11",
                        "r1bq1rk1/pp3ppp/2n1p3/2Pp4/3Pn3/2BB1N2/PPQ2PPP/2KR3R w KQ - 4 11",
                        "e1c1",
                        false,
                        true,
                        false
                },
                // king (castle bs) #5
                MutationTestCase{
                        "r3k2r/pp3ppp/4b1q1/2bpQ3/3N4/P1P1B3/1P3PPP/R4RK1 b kq - 4 16",
                        "r4rk1/pp3ppp/4b1q1/2bpQ3/3N4/P1P1B3/1P3PPP/R4RK1 b kq - 4 16",
                        "e8g8",
                        false,
                        true,
                        true
                },
                // king (castle bl) #6
                MutationTestCase{
                        "r3kb1r/pppq1ppp/5n2/n3p1N1/8/2N4P/PPQ2PP1/R1B2RK1 b kq - 5 13",
                        "2kr1b1r/pppq1ppp/5n2/n3p1N1/8/2N4P/PPQ2PP1/R1B2RK1 b kq - 5 13",
                        "e8c8",
                        false,
                        true,
                        false
                },
                // pawn #1
                MutationTestCase{
                        "r3k2r/8/8/4p3/8/8/8/R3K2R w - - 0 1",
                        "r3k2r/8/8/8/4p3/8/8/R3K2R w - - 0 1",
                        "e5e4"
                },
                // pawn (e-p) #2
                MutationTestCase{
                        "r3k2r/8/8/4pP2/8/8/8/R3K2R w - - 0 1",
                        "r3k2r/8/4P3/8/8/8/8/R3K2R w - - 0 1",
                        "f5e6",
                        true,
                        false,
                        false,
                        true
                },
                // pawn (=Q) #3
                MutationTestCase{
                        "8/3P4/8/8/8/8/8/8 w - - 0 1",
                        "3Q4/8/8/8/8/8/8/8 w - - 0 1",
                        "d7d8",
                        false,
                        false,
                        false,
                        false,
                        QUEEN
                },
                // pawn (=R) #4
                MutationTestCase{
                        "8/3P4/8/8/8/8/8/8 w - - 0 1",
                        "3R4/8/8/8/8/8/8/8 w - - 0 1",
                        "d7d8",
                        false,
                        false,
                        false,
                        false,
                        ROOK
                },
                // pawn (=N) #5
                MutationTestCase{
                        "8/3P4/8/8/8/8/8/8 w - - 0 1",
                        "3N4/8/8/8/8/8/8/8 w - - 0 1",
                        "d7d8",
                        false,
                        false,
                        false,
                        false,
                        KNIGHT
                },
                // pawn (=B) #6
                MutationTestCase{
                        "8/3P4/8/8/8/8/8/8 w - - 0 1",
                        "3B4/8/8/8/8/8/8/8 w - - 0 1",
                        "d7d8",
                        false,
                        false,
                        false,
                        false,
                        BISHOP
                },
        }
};

bool evaluate_mutation_test_case(const MutationTestCase * tc, bool hard) {

    Board start = fen_to_board(tc->start);
    Move m = unpack_four_char_san(tc->move);

    if (is_sentinel(m)) {
        return false;
    }

    // set the information provided, if using mutate()
    if (!hard) {
        tc->is_cap ? m.set_cap() : m.unset_cap();
        tc->is_cas ? m.set_cas() : m.unset_cas();
        tc->is_cas_short ? m.set_cas_short() : m.unset_cas_short();
        tc->is_ep ? m.set_ep() : m.unset_ep();
    }

    // set the prom piece regardless - it's not possible to mutate the board
    // without knowing the promotion piece.
    if (tc->prom_piece != KING) {
        m.set_prom();
        m.set_prom_piece(tc->prom_piece);
    }

    // call the appropriate function
    if (hard) {
        start.mutate_hard(m);
    } else {
        start.mutate(m);
    }

    return board_to_fen(start).compare(tc->end) == 0;
}

bool evaluate_mutate_hard_test_case(const MutationTestCase * tc) {
    return evaluate_mutation_test_case(tc, true);
}

bool evaluate_mutate_test_case(const MutationTestCase * tc) {
    return evaluate_mutation_test_case(tc, false);
}

bool test_mutate_hard() {
    return evaluate_test_set(&mutation_test_set, &evaluate_mutate_hard_test_case);
}

bool test_mutate() {
    TestSet<MutationTestCase> renamed_test_set{
        "board-mutate",
        mutation_test_set.cases
    };
    return evaluate_test_set(&renamed_test_set, &evaluate_mutate_test_case);
}
