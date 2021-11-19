#include "../test.h"
#include "board.h"

struct MutationTestCase {
    const std::string start;
    const std::string end;
    const std::string move;
    const Ptype prom_piece = QUEEN;
};

const TestSet<MutationTestCase> mutate_hard_test_set{
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
                        "d3d7"
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
                        "a7d7"
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
                        "g4d7"
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
                        "f8d7"
                },
                // king #1
                MutationTestCase{
                        "8/8/8/q7/8/8/4bn2/4K3 w - - 0 1",
                        "8/8/8/q7/8/8/4Kn2/8 w - - 0 1",
                        "e1e2"
                },
                // king #2
                MutationTestCase{
                        "8/8/8/q7/8/8/4bn2/4K3 w - - 0 1",
                        "8/8/8/q7/8/8/4bK2/8 w - - 0 1",
                        "e1f2"
                },
                // king (castle ws) #3
                MutationTestCase{
                        "r3kb1r/pp3ppp/4b1q1/3pQ3/3N4/P1P5/1P3PPP/R1B1K2R w KQkq - 1 15",
                        "r3kb1r/pp3ppp/4b1q1/3pQ3/3N4/P1P5/1P3PPP/R1B2RK1 w KQkq - 1 15",
                        "e1g1"
                },
                // king (castle wl) #4
                MutationTestCase{
                        "r1bq1rk1/pp3ppp/2n1p3/2Pp4/3Pn3/2BB1N2/PPQ2PPP/R3K2R w KQ - 4 11",
                        "r1bq1rk1/pp3ppp/2n1p3/2Pp4/3Pn3/2BB1N2/PPQ2PPP/2KR3R w KQ - 4 11",
                        "e1c1"
                },
                // king (castle bs) #5
                MutationTestCase{
                        "r3k2r/pp3ppp/4b1q1/2bpQ3/3N4/P1P1B3/1P3PPP/R4RK1 b kq - 4 16",
                        "r4rk1/pp3ppp/4b1q1/2bpQ3/3N4/P1P1B3/1P3PPP/R4RK1 b kq - 4 16",
                        "e8g8"
                },
                // king (castle bl) #6
                MutationTestCase{
                        "r3kb1r/pppq1ppp/5n2/n3p1N1/8/2N4P/PPQ2PP1/R1B2RK1 b kq - 5 13",
                        "2kr1b1r/pppq1ppp/5n2/n3p1N1/8/2N4P/PPQ2PP1/R1B2RK1 b kq - 5 13",
                        "e8c8"
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
                        "f5e6"
                },
                // pawn (=Q) #3
                MutationTestCase{
                        "8/3P4/8/8/8/8/8/8 w - - 0 1",
                        "3Q4/8/8/8/8/8/8/8 w - - 0 1",
                        "d7d8",
                        QUEEN
                },
                // pawn (=R) #4
                MutationTestCase{
                        "8/3P4/8/8/8/8/8/8 w - - 0 1",
                        "3R4/8/8/8/8/8/8/8 w - - 0 1",
                        "d7d8",
                        ROOK
                },
                // pawn (=N) #5
                MutationTestCase{
                        "8/3P4/8/8/8/8/8/8 w - - 0 1",
                        "3N4/8/8/8/8/8/8/8 w - - 0 1",
                        "d7d8",
                        KNIGHT
                },
                // pawn (=B) #6
                MutationTestCase{
                        "8/3P4/8/8/8/8/8/8 w - - 0 1",
                        "3B4/8/8/8/8/8/8/8 w - - 0 1",
                        "d7d8",
                        BISHOP
                },
        }
};

const TestSet<MutationTestCase> mutate_test_set{
        "board-mutate",
        {
                MutationTestCase{
                        "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",
                },
                MutationTestCase{
                        "r3k2r/8/8/8/8/8/8/R3KN1R w KQkq - 0 1",
                },
                MutationTestCase{
                        "r3k2r/8/8/8/8/8/8/R2bK2R w KQkq - 0 1",
                },
        }
};

bool evaluate_mutation_test_case(const MutationTestCase * tc, bool hard) {

    Board start = fen_to_board(tc->start);
    Move m = unpack_four_char_san(tc->move);

    if (is_sentinel(m)) {
        return false;
    }

    m.set_prom_piece(tc->prom_piece);

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
    return evaluate_test_set(&mutate_hard_test_set, &evaluate_mutate_hard_test_case);
}

bool test_mutate() {
    return evaluate_test_set(&mutate_test_set, &evaluate_mutate_test_case);
}
