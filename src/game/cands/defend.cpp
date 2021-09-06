#include "board.h"
#include "cands.h"
#include "game.h"

#include <iostream>
using std::cout;

/**
 * TODO
 *  - don't defend with pieces which already defend
 *  - make defending with a piece which is on that square a separate response (or stop it)
 *  - discovered defences not supported
 *  - x-rays!
 */

/*
 * Walks out from the piece looking for other pieces which can move to the squares encountered
 * and therefore can defend the piece. Does not consider discovered defences or promotions.
 */
void defend_square(const Board & b, const FeatureFrame * ff, MoveSet * m, int & move_counter) {

    const Square s = ff->centre;
    const Ptype defending_colour = b.get_white() ? WHITE : BLACK;

    Square piece_squares[16];
    int pieces_point = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Square temp = mksq(x, y);
            if (colour(b.get(temp)) == defending_colour && temp != s) {
                piece_squares[pieces_point++] = temp;
            }
        }
    }

    int x, y;
    Square temp;

    // go through the delta pairs entailing each sliding direction
    MoveType dir = DIAG;
    for (int i = 0; i < 8; ++i) {

        if (i == 4) {
            dir = ORTHO;
        }

        const int x_inc = XD[i], y_inc = YD[i];
        x = get_x(s) + x_inc, y = get_y(s) + y_inc;

        // and go in that direction
        while (val(temp = mksq(x, y))) {

            Piece p = b.get(temp);

            if (p == EMPTY) {
                // check for any piece which can move here
                for (int j = 0; j < pieces_point; ++j) {
                    if (can_move_to_square(b, piece_squares[j], temp) && can_move_in_direction(b.get(piece_squares[j]), dir)) {
                        if (move_counter < MAX_MOVES_PER_HOOK) {
                            m->moves[move_counter++] = {piece_squares[j], temp, 0};
                        } else {
                            // no space remaining
                            return;
                        }
                    }
                }
            } else {
                // blocking piece: abort
                break;
            }

            // square was empty, continue
            x += x_inc;
            y += y_inc;

        }

    }

//    cout << "After sliding pieces:\n";
//    for (int i = 0; i < move_counter; ++i) {
//        Move move = m->moves[i];
//        cout << "Move from " << sqtos(move.from) << " to " << sqtos(move.to) << "\n";
//    }

    x = get_x(s), y = get_y(s);

    // knights
    for (int i = 0; i < 8; ++i) {

        // first get the square it would be defending from
        const Square defend_from_square = mksq(x + XKN[i], y + YKN[i]);
        if (val(defend_from_square) && b.get(defend_from_square) == EMPTY) {

            // then check for knights which can move to that square
            for (int j = 0; j < 8; ++j) {
                if (val(temp = mksq(x + XKN[i] + XKN[j], y + YKN[i] + YKN[j]))
                        && (type(b.get(temp)) == KNIGHT)
                        && (colour(b.get(temp)) == defending_colour)
                        && (temp != s)) {
                    if (move_counter < MAX_MOVES_PER_HOOK) {
                        m->moves[move_counter++] = Move{temp, defend_from_square, 0};
                    } else {
                        // no space remaining
                        return;
                    }
                }
            }
        }
    }

    // kings
    for (int i = 0; i < 8; ++i) {

        // check the possible square that a king could defend from
        const Square defend_from_square = mksq(x + XD[i], y + YD[i]);
        if (val(defend_from_square) && b.get(defend_from_square) == EMPTY) {

            // and check the surrounding squares for kings
            for (int j = 0; j < 8; ++j) {
                if (val(temp = mksq(x + XD[i] + XD[j], y + YD[i] + YD[j]))
                        && (type(b.get(temp)) == KING)
                        && (colour(b.get(temp)) == defending_colour)
                        && (temp != s)) {
                    if (move_counter < MAX_MOVES_PER_HOOK) {
                        m->moves[move_counter++] = Move{temp, defend_from_square, 0};
                    } else {
                        // no space remaining
                        return;
                    }
                }
            }
        }
    }

    // pawns
    Square left_pawn_defence_square;
    Square right_pawn_defence_square;
    if (defending_colour == WHITE) {
        left_pawn_defence_square = mksq(x - 1, y - 1);
        right_pawn_defence_square = mksq(x + 1, y - 1);
    } else {
        left_pawn_defence_square = mksq(x - 1, y + 1);
        right_pawn_defence_square = mksq(x + 1, y + 1);
    }

    for (int i = 0; i < pieces_point; ++i) {
        // check that squares are empty?
        if (type(b.get(piece_squares[i])) == PAWN) {
            if (val(left_pawn_defence_square)
                    && can_move_to_square(b, piece_squares[i], left_pawn_defence_square)) {
                if (move_counter < MAX_MOVES_PER_HOOK) {
                    m->moves[move_counter++] = Move{piece_squares[i], left_pawn_defence_square, 0};
                } else {
                    // no space remaining
                    return;
                }
            } else if (val(right_pawn_defence_square)
                        && can_move_to_square(b, piece_squares[i], right_pawn_defence_square)) {
                if (move_counter < MAX_MOVES_PER_HOOK) {
                    m->moves[move_counter++] = Move{piece_squares[i], right_pawn_defence_square, 0};
                } else {
                    // no space remaining
                    return;
                }
            }

        }
    }

    return;

}
