#include <iostream>
using std::cout;

#include "game.h"
#include "../cands/cands.h"
#include "../heur/heur.h"

/*
 * Walks along the board in the given manner, maintaining a running total of the +- control
 * of the given square on the board. Updates min_w/min_b if it encounters a threatening piece
 * of the relevant colour of lower value than the current value of min_w/min_b.
 */
bool capture_walk(const Board & b, Square s) {
    
    int balance = 0;
    int min_value_w = piece_value(W_KING)*10;
    int min_value_b = piece_value(W_KING)*10;
    int x, y;
    Square temp;

    if (type(b.get(s)) == EMPTY) { return false; }

    // go through the delta pairs entailing each sliding direction
    MoveType dir = DIAG;
    for (int i = 0; i < 8; ++i) {
        
        if (i == 4) {
            dir = ORTHO;
        }
        
        int x_inc = XD[i], y_inc = YD[i];
        bool cont = true;
        bool x_ray = false;

        x = get_x(s) + x_inc, y = get_y(s) + y_inc;

        // and go in that direction
        while (val(temp = mksq(x, y)) && cont) {
    
            Piece p = b.get(temp);

            if ((type(p) != EMPTY) && can_move(p, dir)) {
                // any piece which can move in the right dir: account and continue
                int val = piece_value(p);
                if (colour(p) == WHITE) {
                    ++balance;
                    if (!x_ray && val < min_value_w) {
                        min_value_w = val;
                    }
                } else {
                    --balance;
                    if (!x_ray && val < min_value_b) {
                        min_value_b = val;
                    }
                }
                x_ray = true;
            } else  if (type(p) != EMPTY) {
                // blocking piece: abort
                cont = false;
            }
            
            x += x_inc;
            y += y_inc;
            
        }
        
    }

    x = get_x(s);
    y = get_y(s);

    // knights
    int kn_val = piece_value(W_KNIGHT);
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + XKN[i], y + YKN[i])) && (type(b.get(temp)) == KNIGHT)) {
            if (colour(b.get(temp)) == WHITE) {
                ++balance;
                if (min_value_w > kn_val) {
                    min_value_w = kn_val;
                }
            } else {
                --balance;
                if (min_value_b > kn_val) {
                    min_value_b = kn_val;
                }
            }
        }
    }

    // kings
    int k_val = piece_value(W_KING);
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + XD[i], y + YD[i])) && (type(b.get(temp)) == KING)) {
            if (colour(b.get(temp)) == WHITE) {
                ++balance;
                if (min_value_w > k_val) {
                    min_value_w = k_val;
                }
            } else {
                --balance;
                if (min_value_b > k_val) {
                    min_value_b = k_val;
                }
            }
        }
    }

    // pawns
    int pawn_val = piece_value(W_PAWN);
    if (val(temp = mksq(x + 1, y + 1)) && (b.get(temp) == B_PAWN)) {
        balance -= 1;
        if (min_value_b > pawn_val) {
            min_value_b = pawn_val;
        }
    }
    if (val(temp = mksq(x - 1, y + 1)) && (b.get(temp) == B_PAWN)) {
        balance -= 1;
        if (min_value_b > pawn_val) {
            min_value_b = pawn_val;
        }
    }
    if (val(temp = mksq(x + 1, y - 1)) && (b.get(temp) == W_PAWN)) {
        balance += 1;
        if (min_value_w > pawn_val) {
            min_value_w = pawn_val;
        }
    }
    if (val(temp = mksq(x - 1, y - 1)) && (b.get(temp) == W_PAWN)) {
        balance += 1;
        if (min_value_w > pawn_val) {
            min_value_w = pawn_val;
        }
    }

//    cout << "Balance: " << balance
//            << "\nMin white: " << min_value_w
//            << "\nMin black: " << min_value_b << "\n";

    if (colour(b.get(s)) == WHITE) {
        return (balance < 0 && min_value_b <= piece_value(B_KING)) || (min_value_b < piece_value(b.get(s)));
    } else {
        return (balance > 0 && min_value_w <= piece_value(W_KING)) || (min_value_w < piece_value(b.get(s)));
    }

}

/*
 * Walks out from the given square to find a piece which can capture the weak piece.
 * Selects all pieces which can capture the target and which share the minimum value
 * among such pieces.
 */
void capture_piece(const Board & b, Square s, MoveSet *moves, int & move_counter) {

    const int local_reset_point = move_counter;

    int min_value_seen = piece_value(W_KING) + 1;
    Ptype capturing_colour = (colour(b.get(s)) == WHITE) ? BLACK : WHITE;
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

            if ((type(p) != EMPTY) && can_move(p, dir) && colour(p) == capturing_colour) {
                // piece of the right colour which can move in the right dir: check value
                int val = piece_value(p);
                if (val < min_value_seen) {
                    // new lowest value; reset and add to the list
                    min_value_seen = val;
                    move_counter = local_reset_point;
                    moves->moves[move_counter++] = Move{temp, s, 0};
                } else if (val == min_value_seen) {
                    // equal lowest value; append
                    if (move_counter < MAX_MOVES_PER_HOOK) {
                        moves->moves[move_counter++] = Move{temp, s, 0};
                    }
                }
                // no x-rays!
                break;
            } else  if (type(p) != EMPTY) {
                // blocking piece: abort
                break;
            }

            x += x_inc;
            y += y_inc;

        }

    }

    x = get_x(s);
    y = get_y(s);

    // knights
    int kn_val = piece_value(W_KNIGHT);
    if (min_value_seen >= kn_val) {
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
                    if (move_counter < MAX_MOVES_PER_HOOK) {
                        moves->moves[move_counter++] = Move{temp, s, 0};
                    }
                }
            }
        }
    }

    // kings
    int k_val = piece_value(W_KING);
    if (min_value_seen >= k_val) {
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
                    if (move_counter < MAX_MOVES_PER_HOOK) {
                        moves->moves[move_counter++] = Move{temp, s, 0};
                    }
                }
            }
        }
    }

    // pawns
    int pawn_val = piece_value(W_PAWN);
    if (val(temp = mksq(x + 1, y + 1)) && (b.get(temp) == B_PAWN) && (capturing_colour == BLACK)) {
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            move_counter = local_reset_point;
            moves->moves[move_counter++] = Move{temp, s, 0};
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (move_counter < MAX_MOVES_PER_HOOK) {
                moves->moves[move_counter++] = Move{temp, s, 0};
            }
        }
    }
    if (val(temp = mksq(x - 1, y + 1)) && (b.get(temp) == B_PAWN) && (capturing_colour == BLACK)) {
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            move_counter = local_reset_point;
            moves->moves[move_counter++] = Move{temp, s, 0};
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (move_counter < MAX_MOVES_PER_HOOK) {
                moves->moves[move_counter++] = Move{temp, s, 0};
            }
        }
    }
    if (val(temp = mksq(x + 1, y - 1)) && (b.get(temp) == W_PAWN) && (capturing_colour == WHITE)) {
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            move_counter = local_reset_point;
            moves->moves[move_counter++] = Move{temp, s, 0};
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (move_counter < MAX_MOVES_PER_HOOK) {
                moves->moves[move_counter++] = Move{temp, s, 0};
            }
        }
    }
    if (val(temp = mksq(x - 1, y - 1)) && (b.get(temp) == W_PAWN) && (capturing_colour == WHITE)) {
        if (pawn_val < min_value_seen) {
            // new lowest value; reset and add to the list
            min_value_seen = pawn_val;
            move_counter = local_reset_point;
            moves->moves[move_counter++] = Move{temp, s, 0};
        } else if (pawn_val == min_value_seen) {
            // equal lowest value; append
            if (move_counter < MAX_MOVES_PER_HOOK) {
                moves->moves[move_counter++] = Move{temp, s, 0};
            }
        }
    }

}

void defend_piece(const Board & b, Square s, MoveSet * m, int & move_counter) {

}

void weak_hook(const Board & b, FeatureFrame** frame) {

    Square hits[64];
    int i = 0;

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (capture_walk(b, mksq(x, y))) {
                hits[i++] = mksq(x, y);
            }
        }
    }

    *frame = static_cast<FeatureFrame*> (operator new((sizeof(FeatureFrame)) * (i + 1)));

    for (int j = 0; j < i; ++j) {
        (*frame)[j].centre = hits[j];
        (*frame)[j].secondary = 0;
    }
    (*frame)[i].centre = SQUARE_SENTINEL;
    (*frame)[i].secondary = SQUARE_SENTINEL;
}

void weak_resp(const Board & b, MoveSet* moves, FeatureFrame* frame) {
    // check frame and turn, delegate to major and minor adding moves.
    bool white_to_play = b.get_white();
    int move_count = 0;

    for (FeatureFrame* ff = frame; ff->centre != SQUARE_SENTINEL; ff++) {
        bool weak_piece_is_white = (colour(b.get(ff->centre)) == WHITE);
        if (white_to_play == weak_piece_is_white) {
            defend_piece(b, ff->centre, moves, move_count);
        } else {
            capture_piece(b, ff->centre, moves, move_count);
        }
        if (move_count == MAX_MOVES_PER_HOOK) {
            return;
        }
    }

    if (move_count < MAX_MOVES_PER_HOOK) {
        moves->moves[move_count] = MOVE_SENTINEL;
    }
}
