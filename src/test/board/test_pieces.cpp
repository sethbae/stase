#include "../test.h"
#include "board.h"

struct PieceTestCase {
    Piece p;
    Ptype exp_type;
    Ptype exp_colour;
};

const TestSet<PieceTestCase> piece_test_set{
    "board-piece-types",
    {
        PieceTestCase{B_KING, KING, BLACK},
        PieceTestCase{B_QUEEN, QUEEN, BLACK},
        PieceTestCase{B_ROOK, ROOK, BLACK},
        PieceTestCase{B_BISHOP, BISHOP, BLACK},
        PieceTestCase{B_KNIGHT, KNIGHT, BLACK},
        PieceTestCase{B_PAWN, PAWN, BLACK},
        PieceTestCase{W_KING, KING, WHITE},
        PieceTestCase{W_QUEEN, QUEEN, WHITE},
        PieceTestCase{W_ROOK, ROOK, WHITE},
        PieceTestCase{W_BISHOP, BISHOP, WHITE},
        PieceTestCase{W_KNIGHT, KNIGHT, WHITE},
        PieceTestCase{W_PAWN, PAWN, WHITE},
    }
};

bool evaluate_piece_test_case(const PieceTestCase * tc) {
    return type(tc->p) == tc->exp_type && colour(tc->p) == tc->exp_colour;
}

bool test_pieces() {
    return evaluate_test_set(&piece_test_set, &evaluate_piece_test_case);
}
