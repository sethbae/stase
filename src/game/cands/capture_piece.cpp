#include "../heur/heur.h"
#include "cands.h"
#include "game.h"

// void capture_piece(const Board & b, Square s, MoveSet *moves, int & move_counter, int & minw, int & minb) {
void capture_piece(const Board & b, const FeatureFrame * ff, MoveSet * moves, int & move_counter) {

    const Square s = ff->centre;
    const int local_reset_point = move_counter;
    const Ptype capturing_colour = (colour(b.get(s)) == WHITE) ? BLACK : WHITE;
    const int weakest_defender = (capturing_colour == WHITE) ? ff->conf_2 : ff->conf_1;

    int min_value_seen = piece_value(W_KING) + 1;
    int x, y;
    Square temp;

    // go through the delta pairs entailing each sliding direction
    MoveType dir = DIAG;
    for (int i = 0; i < 8; ++i) {

        if (i == 4) {
            dir = ORTHO;
        }

        int x_inc = XD[i], y_inc = YD[i];
        x = get_x(s) + x_inc, y = get_y(s) + y_inc;

        // and go in that direction
        while (val(temp = mksq(x, y))) {

            Piece p = b.get(temp);

            if ((type(p) != EMPTY) && can_move_in_direction(p, dir) && colour(p) == capturing_colour) {
                // piece of the right colour which can move in the right dir: check value
                int val = piece_value(p);
                if (val <= weakest_defender) {
                    if (val < min_value_seen) {
                        // new lowest value; reset and add to the list
                        min_value_seen = val;
                        move_counter = local_reset_point;
                        moves->moves[move_counter++] = Move{temp, s, 0};
                    } else if (val == min_value_seen) {
                        // equal lowest value; append
                        if (move_counter < MAX_MOVES_PER_HOOK &&
                                move_counter - local_reset_point < MAX_MOVES_PER_RESPONDER) {
                            moves->moves[move_counter++] = Move{temp, s, 0};
                        }
                    }
                }
                // no x-rays!
                break;
            } else if (type(p) != EMPTY) {
                // blocking piece: abort
                break;
            }

            // square was empty, continue
            x += x_inc;
            y += y_inc;

        }

    }

    x = get_x(s);
    y = get_y(s);

    // knights
    int kn_val = piece_value(W_KNIGHT);
    if (min_value_seen >= kn_val && weakest_defender >= kn_val) {
        for (int i = 0; i < 8; ++i) {
            if (val(temp = mksq(x + XKN[i], y + YKN[i]))
                && (type(b.get(temp)) == KNIGHT)
                && (colour(b.get(temp)) == capturing_colour)) {
                if (kn_val < min_value_seen) {
                    // new lowest value; reset and add to the list
                    min_value_seen = kn_val;
                    move_counter = local_reset_point;
                    moves->moves[move_counter++] = Move{temp, s, 0};
                } else if (kn_val == min_value_seen) {
                    // equal lowest value; append
                    if (move_counter < MAX_MOVES_PER_HOOK &&
                            move_counter - local_reset_point < MAX_MOVES_PER_RESPONDER) {
                        moves->moves[move_counter++] = Move{temp, s, 0};
                    }
                }
            }
        }
    }

    // kings
    int k_val = piece_value(W_KING);
    if (min_value_seen >= k_val && weakest_defender >= k_val) {
        for (int i = 0; i < 8; ++i) {
            if (val(temp = mksq(x + XD[i], y + YD[i]))
                && (type(b.get(temp)) == KING)
                && (colour(b.get(temp)) == capturing_colour)) {
                if (k_val < min_value_seen) {
                    // new lowest value; reset and add to the list
                    min_value_seen = k_val;
                    move_counter = local_reset_point;
                    moves->moves[move_counter++] = Move{temp, s, 0};
                } else if (k_val == min_value_seen) {
                    // equal lowest value; append
                    if (move_counter < MAX_MOVES_PER_HOOK &&
                            move_counter - local_reset_point < MAX_MOVES_PER_RESPONDER) {
                        moves->moves[move_counter++] = Move{temp, s, 0};
                    }
                }
            }
        }
    }

    // pawns
    int pawn_val = piece_value(W_PAWN);
    if (val(temp = mksq(x + 1, y + 1)) && (b.get(temp) == B_PAWN) && (capturing_colour == BLACK)) {
        // IMPLICIT: pawns are always <= weakest defender
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            move_counter = local_reset_point;
            moves->moves[move_counter++] = Move{temp, s, 0};
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (move_counter < MAX_MOVES_PER_HOOK && move_counter - local_reset_point < MAX_MOVES_PER_RESPONDER) {
                moves->moves[move_counter++] = Move{temp, s, 0};
            }
        }
    }
    if (val(temp = mksq(x - 1, y + 1)) && (b.get(temp) == B_PAWN) && (capturing_colour == BLACK)) {
        // IMPLICIT: pawns are always <= weakest defender
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            move_counter = local_reset_point;
            moves->moves[move_counter++] = Move{temp, s, 0};
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (move_counter < MAX_MOVES_PER_HOOK && move_counter - local_reset_point < MAX_MOVES_PER_RESPONDER) {
                moves->moves[move_counter++] = Move{temp, s, 0};
            }
        }
    }
    if (val(temp = mksq(x + 1, y - 1)) && (b.get(temp) == W_PAWN) && (capturing_colour == WHITE)) {
        // IMPLICIT: pawns are always <= weakest defender
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            move_counter = local_reset_point;
            moves->moves[move_counter++] = Move{temp, s, 0};
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (move_counter < MAX_MOVES_PER_HOOK && move_counter - local_reset_point < MAX_MOVES_PER_RESPONDER) {
                moves->moves[move_counter++] = Move{temp, s, 0};
            }
        }
    }
    if (val(temp = mksq(x - 1, y - 1)) && (b.get(temp) == W_PAWN) && (capturing_colour == WHITE)) {
        // IMPLICIT: pawns are always <= weakest defender
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            move_counter = local_reset_point;
            moves->moves[move_counter++] = Move{temp, s, 0};
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (move_counter < MAX_MOVES_PER_HOOK && move_counter - local_reset_point < MAX_MOVES_PER_RESPONDER) {
                moves->moves[move_counter++] = Move{temp, s, 0};
            }
        }
    }

}
