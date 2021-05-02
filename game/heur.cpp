
#include "game.h"
#include "../board/board.h"

#include <iostream>
using std::cout;
#include <string>
using std::string;

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
Metric piece_activity_beta;
Metric piece_activity_gamma;
Metric open_line_control;
Metric centre_control;
Metric pawn_struct;

const Metric* METRICS[] = {
    &piece_activity_alpha,
    &piece_activity_beta,
    &piece_activity_gamma,
    &open_line_control,
    &centre_control,
    // &pawn_struct
};

const string HEUR_NAMES[] = {
    "Alpha activity    ",
    "Beta activity     ", 
    "Gamma activity    ", 
    "Open line control ",
    "Centre control    "
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
    600,   // piece activity: alpha
    600,    // beta
    600,    // gamma
    1000,   // open line
    1500,   // centre control
    // 1000    // pawn_struct
};

/*
 * Specifies how many metrics to use. Very unsafe - there is no bounds checking used.
 */
const unsigned METRICS_IN_USE = 5;


// the central 16 squares
const Square CENTRAL_SQUARES[] = {
    stosq("c6"), stosq("d6"), stosq("e6"), stosq("f6"),
    stosq("c5"), stosq("d5"), stosq("e5"), stosq("f5"),
    stosq("c4"), stosq("d4"), stosq("e4"), stosq("f4"),
    stosq("c3"), stosq("d3"), stosq("e3"), stosq("f3")
};
const unsigned NUM_CENTRAL_SQUARES = 16; // perhaps unnecessary

// the central four squares
const Square INNER_CENTRAL[] = {

                 stosq("d5"), stosq("e5"),
                 stosq("d4"), stosq("e4"),
                 
};
const unsigned NUM_INNER_CENTRAL = 4; // perhaps unnecessary

// the next ring of 12 central squares
const Square OUTER_CENTRAL[] = {
    stosq("c6"), stosq("d6"), stosq("e6"), stosq("f6"),
    stosq("c5"),                           stosq("f5"),
    stosq("c4"),                           stosq("f4"),
    stosq("c3"), stosq("d3"), stosq("e3"), stosq("f3")
};
const unsigned NUM_OUTER_CENTRAL = 12;

// returns a positive integer representing the value of the piece
int piece_value(const Piece p) {
    
    switch (type(p)) {
        case PAWN: return 1000;
        case KNIGHT:
        case BISHOP: return 3000;
        case ROOK: return 5000;
        default: return 9000;
    }   
    
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

Eval heur(const Gamestate & gs) {
   
    
    int ev = 0;
    
    for (unsigned i = 0; i < METRICS_IN_USE; ++i) {
        int score = METRICS[i](gs.board) * WEIGHTS[i];
        ev += score;
    }
    
    return ((Eval)ev) + material_balance(gs);
}

Eval heur_with_description(const Gamestate & gs) {

    cout << "Evaluating position:\n";
    pr_board(gs.board);
    
    cout << "Contributions for each metric (millipawns):\n";
    
    int mat_bal = material_balance(gs.board);
    cout << "Material balance  : " << mat_bal << "\n";

    int ev = 0;
    for (unsigned i = 0; i < METRICS_IN_USE; ++i) {
        float score = METRICS[i](gs.board);
        int weighted_score = score * WEIGHTS[i];
        ev += weighted_score;
        
        cout << HEUR_NAMES[i] << ": " << score 
                << " * " << WEIGHTS[i] 
                << " = " << weighted_score
                << "\n";
    }
    
    Eval final_ev = ((Eval)ev) + mat_bal;
    
    cout << "\nFinal evaluation  : " << human_eval(final_ev) << "\n";
    return final_ev;

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
    
    float w_proportion = ((float)w_control / (float)(b_control + w_control));
    
    return -1.0 + 2*w_proportion;
    
}

// make the central squares count even more!
float centre_control(const Board & b) {
    
    int count = 0;
    
    for (unsigned i = 0; i < NUM_INNER_CENTRAL; ++i) {
        count += 2*control_count(b, INNER_CENTRAL[i]);
    }
    
    for (unsigned i = 0; i < NUM_OUTER_CENTRAL; ++i) {
        count += control_count(b, OUTER_CENTRAL[i]);
    }
    
    // my thought with normalising over 20 was that 12 + 2x4 = 20, so that represents having 
    // control over every square. Obviousy this can overflow.
    return ((float) count) / 20.0;
    
}

// counts the number of pieces on the given rank, file or diagonal which can move in that
// direction. Ignores other pieces (notably pawns). It increments for white, and decrements
// for black.
int line_control(const Board & b, Square s, StepFunc *step, MoveType dir) {
    
    int control = 0;
    
    while (val(s)) {

        Piece p = b.get(s);
        if (can_move(p, dir)) {
            control += ((colour(p) == WHITE) ? 1 : -1);
        }
        (*step)(s);
    }
    
    return control;
}

// counts the number of pawns reachable from the given square according to the step func,
// ie the number which are on a certain rank or file
unsigned count_pawns(const Board & b, Square s, StepFunc *step) {
    
    unsigned sum = 0;
    
    while (val(s)) {

        Piece p = b.get(s);
        if (type(p) == PAWN) {
            ++sum;
        }
        (*step)(s);
    }
    
    return sum;
}

// who controls the open lines and diagonals? This checks ranks, files and major diagonals
float open_line_control(const Board & b) {
 
    // TODO: this only counts lines which have no pawns on. For something like the 
    // seventh rank, this is rarely appropriate - some more sophisticated measure is needed.

    int score = 0;
 
    unsigned FILE_VALUES[] = {
     // a  b  c  d  e  f  g  h
        1, 2, 3, 3, 3, 3, 2, 1
    };
    
    unsigned RANK_VALUES[] = {
  // y =0  1  2  3  4  5  6  7  
        3, 2, 1, 1, 1, 1, 2, 3
    };
    
    unsigned DIAG_VALUES[] = {
    // a2  a1 b1
        1, 3, 1,    // positive 3 diagonals
    // b8  a8 a7    // negative 3 diagonals
        1, 3, 1
    };
 
    // files 
    for (unsigned x = 0; x < 8; ++x) {
        Square sq = mksq(x, 0);
        if (count_pawns(b, sq, inc_y) == 0) {
            score += line_control(b, sq, inc_y, ORTHO) * FILE_VALUES[x];
        }
    }
    
    // ranks
    for (unsigned y = 0; y < 8; ++y) {
        Square sq = mksq(0, y);
        if (count_pawns(b, sq, inc_x) == 0) {
            score += line_control(b, sq, inc_x, ORTHO) * RANK_VALUES[y];
        }
    }
    
    // diagonals
    Square sq = stosq("a2");
    if (count_pawns(b, sq, diag_ur) == 0)
        score += line_control(b, sq, diag_ur, DIAG) * DIAG_VALUES[0];
        
    sq = stosq("a1");
    if (count_pawns(b, sq, diag_ur) == 0)
        score += line_control(b, sq, diag_ur, DIAG) * DIAG_VALUES[1];
    
    sq = stosq("b1");
    if (count_pawns(b, sq, diag_ur) == 0)
        score += line_control(b, sq, diag_ur, DIAG) * DIAG_VALUES[2];
        
    sq = stosq("b8");
    if (count_pawns(b, sq, diag_dr) == 0)
        score += line_control(b, sq, diag_ur, DIAG) * DIAG_VALUES[3];
        
    sq = stosq("a8");
    if (count_pawns(b, sq, diag_dr) == 0)
        score += line_control(b, sq, diag_ur, DIAG) * DIAG_VALUES[4];
    
    sq = stosq("a7");
    if (count_pawns(b, sq, diag_dr) == 0)
        score += line_control(b, sq, diag_ur, DIAG) * DIAG_VALUES[5];
        
    return ((float)score) / 20.0;
        
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
