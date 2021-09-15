#include "cands.h"


/*

 * * * * * * * *
 * * * * * * * *
 * * * * * * * *
 * 5 * * * * 5 *
 * * 4 * * 4 * *
 3 * * 3 3 * * 3
 * 5 * 2 2 * 5 *
 * * B * * B * *



 */
const int BISHOPS[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 2, 0, 2, 2, 0, 2, 0},
        {1, 0, 0, 1, 1, 0, 0, 1},
        {0, 2, 1, 0, 0, 1, 2, 0},
        {0, 2, 1, 0, 0, 1, 2, 0},
        {1, 0, 0, 1, 1, 0, 0, 1},
        {0, 2, 0, 2, 2, 0, 2, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
};

const int KNIGHTS[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
};

const int ROOKS[8][8] = {
        {0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 0, 0},
};

const int QUEENS[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 2, 2, 2, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 1},
        {0, 1, 0, 0, 0, 1, 0, 0},
        {0, 0, 2, 2, 2, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
};

const int XKN_LEGAL_W[] = {1, 2, -1, -2};
const int YKN_LEGAL_W[] = {2, 1, 2, 1};

const int XKN_LEGAL_B[] = {1, 2, -1, -2};
const int YKN_LEGAL_B[] = {-2, -1, -2, -1};

void best_bishop_square(const Board & b, Square s, MoveSet * moves, int & move_counter) {

}

void best_knight_square(const Board & b, Square s, MoveSet * moves, int & move_counter) {

    const bool white = (colour(b.get(s)) == WHITE);
    const int * xd = white ? XKN_LEGAL_W : XKN_LEGAL_B;
    const int * yd = white ? YKN_LEGAL_W : YKN_LEGAL_B;

}

void best_queen_square(const Board & b, Square s, MoveSet * moves, int & move_counter) {

}

void best_rook_square(const Board & b, Square s, MoveSet * moves, int & move_counter) {

}

void develop_piece(const Board & b, FeatureFrame * ff, MoveSet *moves, int & move_counter) {

    bool white_piece = colour(b.get(s)) == WHITE;
    if (b.get_white() != white_piece) {
        return;
    }

    switch (type(b.get(ff->centre))) {
        case BISHOP:
            best_bishop_square(b, ff->centre, moves, move_counter);
            return;
        case KNIGHT:
            best_knight_square(b, ff->centre, moves, move_counter);
            return;
        case ROOK:
            best_rook_square(b, ff->centre, moves, move_counter);
            return;
        case QUEEN:
            best_queen_square(b, ff->centre, moves, move_counter);
            return;
        default:
            return;
    }

}
