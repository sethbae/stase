
#include "game.h"
#include "../board/board.h"

#include <iostream>
using std::cout;

/*
 * Metrics measure concretely some element of the board and score it as in favour of white
 * or black. They should not anticipate pieces moving: tactics should play out elsewhere.
 * Thus it is the (somewhat unreliable) assumption that here, we have a quiescent position,
 * and that the balance can therefore be assessed by things like pawn structure, piece
 * activity, etc.
 *
 * Dynamic factors can still be accounted for of course; such as king safety or far
 * advanced pawns (this is different to accounting for the literal mating combination
 * or the sequence of concrete moves by which a pawn promotes).
 * 
 * Each metric maps the board to a float. They return a positive (resp. negative) 
 * number -1 <= x <= 1 indicating favour to one side or another.
 */
typedef float Metric(const Board &);

/*
 * The order of this list is very important. It should correspond to the order
 * in which weights appear in the weights index below.
 *
 * REMEMBER TO UPDATE THE NUMBER OF METRICS USED (constant defined below)
 */
Metric piece_activity_alpha;
Metric centre_control;
Metric pawn_struct;
const Metric* METRICS[] = {
    &piece_activity_alpha,
    &centre_control,
    &pawn_struct
};

/*
 * Weights are specified in millipawns; 1000 means that the corresponding metric will
 * contribute at most a pawn's worth in either player's favour. If a metric is totally
 * in favour of white (1.0 so to speak), the weight will be added to the evaluation.
 * If it is 0.0 it will have no effect, and if it were -0.57... then it would contribute
 * approximately a half of the weight to black's favour.
 *
 * The weights here are mapped by index, according to the comments and the ordering
 * of the metrics in the array above.
 *
 * REMEMBER TO UPDATE THE NUMBER OF METRICS USED (constant defined below)
 */
const int WEIGHTS[] = {
    2000,   // piece activity
    1500,   // centre control
    1000    // pawn_struct
};

/*
 * Specifies how many metrics to use. Very unsafe - there is no bounds checking used.
 */
const unsigned METRICS_IN_USE = 3;


// the central 16 squares
const Square CENTRAL_SQUARES[] = {
    stosq("c6"), stosq("d6"), stosq("e6"), stosq("f6"),
    stosq("c5"), stosq("d5"), stosq("e5"), stosq("f5"),
    stosq("c4"), stosq("d4"), stosq("e4"), stosq("f4"),
    stosq("c3"), stosq("d3"), stosq("e3"), stosq("f3")
};
const unsigned NUM_CENTRAL_SQUARES = 16; // perhaps unnecessary

// returns a positive integer representing the value of the piece
int piece_value(const Piece p) {
    
    switch (type(p)) {
        case PAWN: return 1;
        case KNIGHT:
        case BISHOP: return 3;
        case ROOK: return 5;
        default: return 9;
    }   
    
}

Eval heur(const Gamestate & gs) {
    
    /*
    int ev = 0;
    
    for (unsigned i = 0; i < METRICS_IN_USE; ++i) {
        int score = METRICS[i](gs.board) * WEIGHTS[i];
        ev += score;
    }
    
    return (Eval) ev;
    */
    int material_balance(const Gamestate &);
    return material_balance(gs);
}

// returns an evaluation of the given board
int material_balance(const Gamestate & gs) {

    Eval count = (Eval) 0;
    
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Piece p = gs.board.get(mksq(x, y));
            
            if (colour(p) == WHITE)
                count += piece_value(p);
            else if (colour(p) == BLACK)
                count -= piece_value(p);
            // colour can be INVALID
        
        }
    }
    
    return count;

}


float piece_activity_alpha(const Board & b) {
    
    int w_control = 0;
    int b_control = 0;
    
    Square sq;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            
            sq = mksq(x, y);
            Piece p = b.get(sq);
            
            if (colour(p) == WHITE) {
                w_control += alpha_control(b, sq);
            } else if (colour(p) == BLACK) {
                b_control += alpha_control(b, sq);
            }
        }
    }
    
    cout << "White: " << w_control << "\n";
    cout << "Black: " << b_control << "\n";
    
    float w_proportion = ((float)w_control / (float)(b_control + w_control));
    
    return -1.0 + 2*w_proportion;
    
}

float piece_activity_beta(const Board & b) {
    
    int w_control = 0;
    int b_control = 0;
    
    Square sq;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            
            sq = mksq(x, y);
            Piece p = b.get(sq);
            
            if (colour(p) == WHITE) {
                w_control += beta_control(b, sq);
            } else if (colour(p) == BLACK) {
                b_control += beta_control(b, sq);
            }
        }
    }
    
    cout << "White: " << w_control << "\n";
    cout << "Black: " << b_control << "\n";
    
    float w_proportion = ((float)w_control / (float)(b_control + w_control));
    
    return -1.0 + 2*w_proportion;
    
}

float piece_activity_gamma(const Board & b) {
    
    int w_control = 0;
    int b_control = 0;
    
    Square sq;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            
            sq = mksq(x, y);
            Piece p = b.get(sq);
            
            if (colour(p) == WHITE) {
                w_control += gamma_control(b, sq);
            } else if (colour(p) == BLACK) {
                b_control += gamma_control(b, sq);
            }
        }
    }
    
    cout << "White: " << w_control << "\n";
    cout << "Black: " << b_control << "\n";
    
    float w_proportion = ((float)w_control / (float)(b_control + w_control));
    
    return -1.0 + 2*w_proportion;
    
}

// make the central squares count even more!
float centre_control(const Board & b) {
    
    int count = 0;
    
    for (unsigned i = 0; i < NUM_CENTRAL_SQUARES; ++i) {
        count += control_count(b, CENTRAL_SQUARES[i]);
    }
    
    return ((float) count) / 50.0;
}

float pawn_struct(const Board & b) {
    b.get(mksq(0, 0));
    return 0;
}

/*
float space(const Board & b) {
    return 0;
}

float king_safety(const Board & b) {
    return 0;
}

float development(const Board & b) {
    return 0;
}
*/
