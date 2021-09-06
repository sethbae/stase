#include "board.h"
#include "cands.h"
#include "game.h"

#include <iostream>
using std::cout;

/*
 * Walks out from the piece looking for other pieces which can move to the squares encountered
 * and therefore can defend the piece. Does not consider discovered defences or promotions.
 */
void defend_square(const Board & b, const FeatureFrame * ff, MoveSet * m, int & move_counter) {

    const Square s = ff->centre;
    const Ptype defending_colour = colour(b.get(s));

    // TODO this is painful! Roll on the cache.
    Square piece_squares[16];
    int pieces_point = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Square temp = mksq(x, y);
            if (colour(b.get(temp)) == defending_colour) {
                cout << sqtos(temp) << " ";
                piece_squares[pieces_point++] = temp;
            }
        }
    }
    cout << "\n";

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
                    if (can_move_to_square(b, piece_squares[j], temp) && can_move_in_direction(type(piece_squares[j]), dir)) {
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

    // knights
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + XKN[i], y + YKN[i]))
            && (type(b.get(temp)) == KNIGHT)
            && (colour(b.get(temp)) == defending_colour)) {
            if (move_counter < MAX_MOVES_PER_HOOK) {
                m->moves[move_counter++] = Move{temp, s, 0};
            } else {
                // no space remaining
                return;
            }
        }
    }

    // kings
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + XD[i], y + YD[i]))
            && (type(b.get(temp)) == KING)
            && (colour(b.get(temp)) == defending_colour)) {
            if (move_counter < MAX_MOVES_PER_HOOK) {
                m->moves[move_counter++] = Move{temp, s, 0};
            } else {
                // no space remaining
                return;
            }
        }
    }

    // pawns
    Square left_pawn_defence_square;
    Square right_pawn_defence_square;
    if (defending_colour == WHITE) {
        left_pawn_defence_square = mksq(get_x(s) - 1, get_y(s) - 1);
        right_pawn_defence_square = mksq(get_x(s) + 1, get_y(s) - 1);
    } else {
        left_pawn_defence_square = mksq(get_x(s) - 1, get_y(s) + 1);
        right_pawn_defence_square = mksq(get_x(s) + 1, get_y(s) + 1);
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
