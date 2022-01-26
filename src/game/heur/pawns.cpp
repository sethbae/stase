#include "board.h"
#include "heur.h"
#include "../gamestate.hpp"

// returns the number of pawns which directly defend the given pawn (square),
// relying on the colour of the piece on the square given
int supporting_pawns(const Board & b, Square s) {

    int x = get_x(s);
    int y = get_y(s);
    int count = 0;
    Square temp;

    if (colour(b.get(s)) == WHITE) {

        if (val(temp = mksq(x - 1, y - 1)) && b.get(temp) == W_PAWN)
            ++count;
        if (val(temp = mksq(x + 1, y - 1)) && b.get(temp) == W_PAWN)
            ++count;

    } else {

        if (val(temp = mksq(x - 1, y + 1)) && b.get(temp) == B_PAWN)
            ++count;
        if (val(temp = mksq(x + 1, y + 1)) && b.get(temp) == B_PAWN)
            ++count;

    }

    return count;

}

/*
    Assesses how well defended pawns are. You score a point for every time a pawn defends
    another, and lose a point every time a pawn is undefended (unless it hasn't yet moved)
*/
float defended_pawns_metric(const Gamestate & gs) {

    Square sq;
    int score = 0;

    // for each file
    for (int x = 0; x < 8; ++x) {

        int supporters;

        // find white pawn in this file
        sq = mksq(x, 1);
        while (val(sq) && gs.board.get(sq) != W_PAWN) {
            ++sq.y;
        }

        if (val(sq)) {
            // count pawns supporting this pawn
            supporters = supporting_pawns(gs.board, sq);
            if (supporters)
                score += supporters;    // if there are some, score points

        }

        // find black pawn in this file
        sq = mksq(x, 6);
        while (val(sq) && gs.board.get(sq) != B_PAWN) {
            --sq.y;
        }

        if (val(sq)) {
            supporters = supporting_pawns(gs.board, sq);
            if (supporters)
                score -= supporters;    // same as above, but counting the other way
        }

    }

    return ((float)score) / 5.0f;

}

float isolated_pawns_metric(const Gamestate & gs) {

    int score = 0;

    // true indicates that black/white has a pawn in that file
    bool wpawns[8] = { false, false, false, false, false, false, false, false };
    bool bpawns[8] = { false, false, false, false, false, false, false, false };

    // compute the above arrays
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 7; ++y) {
            if (gs.board.get(mksq(x, y)) == W_PAWN) {
                wpawns[x] = true;
            } else if (gs.board.get(mksq(x, y)) == B_PAWN) {
                bpawns[x] = true;
            }
        }
    }

    // find files which are isolated for white (decrementing)
    if (wpawns[0] && !wpawns[1]) {
        --score;
    }
    for (int x = 1; x < 7; ++x) {
        if (!wpawns[x - 1] && wpawns[x] && !wpawns[x + 1]) {
            --score;
        }
    }
    if (!wpawns[6] && wpawns[7]) {
        --score;
    }

    // find files which are isolated for black (incrementing)
    if (bpawns[0] && !bpawns[1]) {
        ++score;
    }
    for (int x = 1; x < 7; ++x) {
        if (!bpawns[x - 1] && bpawns[x] && !bpawns[x + 1]) {
            ++score;
        }
    }
    if (!bpawns[6] && bpawns[7]) {
        ++score;
    }

    return ((float) score) / 3.0f;

}

float central_pawns_metric(const Gamestate & gs) {

    int score = 0;

    // true indicates that black/white has a pawn in that file
    bool wpawns[8] = { false, false, false, false, false, false, false, false };
    bool bpawns[8] = { false, false, false, false, false, false, false, false };

    // compute the above arrays
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 7; ++y) {
            if (gs.board.get(mksq(x, y)) == W_PAWN) {
                wpawns[x] = true;
            } else if (gs.board.get(mksq(x, y)) == B_PAWN) {
                bpawns[x] = true;
            }
        }
    }

    if (wpawns[2])
        score += 1;
    if (wpawns[3])
        score += 2;
    if (wpawns[4])
        score += 2;
    if (wpawns[5])
        score += 1;

    if (bpawns[2])
        score -= 1;
    if (bpawns[3])
        score -= 2;
    if (bpawns[4])
        score -= 2;
    if (bpawns[5])
        score -= 1;

    return ((float) score) / 5.0f;

}

float far_advanced_pawns_metric(const Gamestate & gs) {

    int score = 0;

    int value[8] = { 0, 4, 2, 1, 1, 2, 4, 0 };

    for (int y = 6; y > 0; --y) {
        for (int x = 0; x < 8; ++x) {

            Piece p = gs.board.get(mksq(x, y));

            if (y > 3 && p == W_PAWN) {
                score += value[y];
            }
            if (y <= 3 && p == B_PAWN) {
                score -= value[y];
            }

        }
    }

    return ((float) score) / 10.0f;

}
