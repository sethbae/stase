
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
Metric defended_pawns;

const Metric* METRICS[] = {
    &piece_activity_alpha,
    &piece_activity_beta,
    &piece_activity_gamma,
    &open_line_control,
    &centre_control,
    &defended_pawns
};

const string HEUR_NAMES[] = {
    "Alpha activity    ",
    "Beta activity     ", 
    "Gamma activity    ", 
    "Open line control ",
    "Centre control    ",
    "Defended pawns    "
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
    500,    // defended pawns
};

/*
 * Specifies how many metrics to use. Very unsafe - there is no bounds checking used.
 */
const unsigned METRICS_IN_USE = 6;


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
    
    float w_proportion = 0.0;
    
    if (w_control + b_control > 0)
        w_proportion = ((float)w_control / (float)(b_control + w_control));
    
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
    
    float w_proportion = 0.0;
    
    if (w_control + b_control > 0)
        w_proportion = ((float)w_control / (float)(b_control + w_control));
    
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
    
    float w_proportion = 0.0;
    
    if (w_control + b_control > 0)
        w_proportion = ((float)w_control / (float)(b_control + w_control));
    
    return -1.0 + 2*w_proportion;
    
}

// make the central squares count even more!
float centre_control(const Board & b) {
    
    int count = 0;
    
    for (unsigned i = 0; i < NUM_INNER_CENTRAL; ++i) {
        
        int control = control_count(b, INNER_CENTRAL[i]);
        if (control > 0) {
            count += 2;
        } else if (control < 0) {
            count -= 2;
        }
    
        // count += 2*control_count(b, INNER_CENTRAL[i]);
        
        //cout << "Score for " << sqtos(INNER_CENTRAL[i]) << ": " 
        //    << control_count(b, INNER_CENTRAL[i]) << "\n";
    }
    
    for (unsigned i = 0; i < NUM_OUTER_CENTRAL; ++i) {
        
        int control = control_count(b, OUTER_CENTRAL[i]);
        if (control > 0) {
            count += 1;
        } else if (control < 0) {
            count -= 1;
        }
    
        // count += 2*control_count(b, OUTER_CENTRAL[i]);
    
        //cout << "Score for " << sqtos(OUTER_CENTRAL[i]) << ": " 
        //    << control_count(b, OUTER_CENTRAL[i]) << "\n";
    }
    
    // my thought with normalising over 20 was that 12 + 2x4 = 20, so that represents having 
    // control over every square. Obviousy this can overflow.
    return ((float) count) / 20.0;
    
}

// Open files: return the number of squares controlled on the given file (which is presumed
// to contain no pawns - tho  this is not checked). 
// Essentially this is just 7 squares for whoever controls the file.
int open_line_walk(const Board & b, Square s, StepFunc *step, MoveType dir) {
    
    int control = 0;
    
    while (val(s)) {

        Piece p = b.get(s);
        if (can_move(p, dir)) {
            control += ((colour(p) == WHITE) ? 1 : -1);
        }
        (*step)(s);
    }
    
    // White conrols the file
    if (control > 0)
        return 7;
        
    // Black controls the file
    if (control < 0)
        return -7;
        
    // nobody does
    return 0;
}

// returns an integer representing the difference in controlled squares on a half open file.
// That is, it counts through the line in  two chunks: before pawn, after pawn. Each half gets 
// its length as score provided that one side has a piece on it (that moves correctly). If a 
// half has pieces of both sides on it, that scores zero. White scores count upwards, black 
// scores count downwards, and the net result is returned.
// See open_line_control
int half_line_walk(const Board & b, Square s, StepFunc *step, MoveType dir) {
    
    int score = 0;      // the overall score which we'll return
    unsigned squares_before_pawn = 0;
    int control = 0;    // who controls this run of squares
    int signum = 0;     // -1 -> black does, 0 -> nobody does, 1 -> white does
    
    // walk up to the pawn
    while (val(s) && (type(b.get(s)) != PAWN)) {
        ++squares_before_pawn;  // counting squares

        Piece p = b.get(s);
        if (can_move(p, dir)) { // and control
            control += (colour(p) == WHITE) ? 1 : -1;
        }
        (*step)(s);
    }
    
    // calculate contribution to score
    if (control < 0)
        --signum;
    else if (control > 0)
        ++signum;
    score += signum * squares_before_pawn;
    
    // and now check who controls the other section
    control = 0;
    (*step)(s);
    while (val(s)) {
        Piece p = b.get(s);
        if (can_move(p, dir)) {
            control += (colour(p) == WHITE) ? 1 : -1;
        }
        (*step)(s);
    }
    
    // and calculate contribution of second half
    signum = 0;
    if (control < 0)
        --signum;
    else if (control > 0)
        ++signum;
    // we already know the length
    score += signum * (7 - squares_before_pawn);
    
    return score;
}

// counts the number of pawns reachable from the given square according to the step func,
// ie the number which are on a certain rank or file
// See open_line_control
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

    /*
    
    This tries to measure who controls the position's open lines. It measures this as follows:
    
        -files are open (no pawns), half open (1 pawn) or closed (2 pawns)
        -files can be controlled by one side or no sides (but not both)
        -different files get different weights (see look up tables)
        
    It examines all ranks, files, and the six longest diagonals, scoring thus:
    
        -open: 7 points to whoever controls it
        -half: for each half, 1 point per square to the side who controls it
        -closed: no points
        
    Points are then scaled by the weights and the whole thing is normalised over 300, a number
    with no real justification but which seems to work ok.
    
    NB the half open files are scored per file, not per half - so black's points on that file
    are subtracted from white's. That's slightly different to how open files work, which lends
    more weight to the open files - which I think is alright.
    
    */

    int score = 0;
    Square sq;
    unsigned num_pawns;
 
    unsigned FILE_VALUES[] = {
     // a  b  c  d  e  f  g  h
        10, 12, 15, 15, 15, 15, 12, 10
    };
    
    unsigned RANK_VALUES[] = {
    // y =0  1   2   3   4   5   6   7
        15, 15, 10, 10, 10, 10, 15, 15
    };
    
    unsigned DIAG_VALUES[] = {
    //  a2  a1  b1
        10, 20, 10,   // positive 3 diagonals
    //  b8  a8  a7 
        10, 20, 10    // negative 3 diagonals
    };
 
    // files 
    for (unsigned x = 0; x < 8; ++x) {
        sq = mksq(x, 0);
        num_pawns = count_pawns(b, sq, inc_y);
        if (num_pawns == 0)
            score += open_line_walk(b, sq, inc_y, ORTHO) * FILE_VALUES[x];
        else if (num_pawns == 1)
            score += half_line_walk(b, sq, inc_y, ORTHO) * FILE_VALUES[x];
    }
    
    // int file_score = score;
    // cout << "File score: " << score << "\n";
    
    // ranks
    for (unsigned y = 0; y < 8; ++y) {
        sq = mksq(0, y);
        num_pawns = count_pawns(b, sq, inc_x);
        if (num_pawns == 0)
            score += open_line_walk(b, sq, inc_x, ORTHO) * RANK_VALUES[y];
        else if (num_pawns == 1)
            score += half_line_walk(b, sq, inc_x, ORTHO) * RANK_VALUES[y];
    }
    
    // int rank_score = score;
    // cout << "Rank score: " << score - file_score << "\n";
    
    // diagonals
    sq = stosq("a2");
    num_pawns = count_pawns(b, sq, diag_ur);
    if (num_pawns == 0)
        score += open_line_walk(b, sq, diag_ur, DIAG) * DIAG_VALUES[0];
    else if (num_pawns == 1)
        score += half_line_walk(b, sq, diag_ur, DIAG) * DIAG_VALUES[0];
    
    sq = stosq("a1");
    num_pawns = count_pawns(b, sq, diag_ur);
    if (num_pawns == 0)
        score += open_line_walk(b, sq, diag_ur, DIAG) * DIAG_VALUES[1];
    else if (num_pawns == 1)
        score += half_line_walk(b, sq, diag_ur, DIAG) * DIAG_VALUES[1];
    
    sq = stosq("b1");
    num_pawns = count_pawns(b, sq, diag_ur);
    if (num_pawns == 0)
        score += open_line_walk(b, sq, diag_ur, DIAG) * DIAG_VALUES[2];
    else if (num_pawns == 1)
        score += half_line_walk(b, sq, diag_ur, DIAG) * DIAG_VALUES[2];
        
    sq = stosq("b8");
    num_pawns = count_pawns(b, sq, diag_dr);
    if (num_pawns == 0)
        score += open_line_walk(b, sq, diag_dr, DIAG) * DIAG_VALUES[3];
    if (num_pawns == 1)
        score += half_line_walk(b, sq, diag_dr, DIAG) * DIAG_VALUES[3];
        
    sq = stosq("a8");
    num_pawns = count_pawns(b, sq, diag_dr);
    if (num_pawns == 0)
        score += open_line_walk(b, sq, diag_dr, DIAG) * DIAG_VALUES[4];
    if (num_pawns == 1)
        score += half_line_walk(b, sq, diag_dr, DIAG) * DIAG_VALUES[4];
    
    sq = stosq("a7");
    num_pawns = count_pawns(b, sq, diag_dr);
    if (num_pawns == 0)
        score += open_line_walk(b, sq, diag_dr, DIAG) * DIAG_VALUES[5];
    if (num_pawns == 1)
        score += half_line_walk(b, sq, diag_dr, DIAG) * DIAG_VALUES[5];
        
    // cout << "Diag score: " << score - rank_score << "\n";
        
    return ((float)score) / 300.0;
        
}

// evaluates the solidity of each side's pawn structure.
//  - how many pawns are defended by other pawns
//  - how many pawns are doubled
//  - how many pawns are isolated

unsigned supporting_pawns(const Board & b, Square s) {
    
    unsigned x = get_x(s);
    unsigned y = get_y(s);
    unsigned count = 0;
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
float defended_pawns(const Board & b) {
    
    Square sq;
    int score = 0;
    
    // for each file
    for (int x = 0; x < 8; ++x) {
        
        unsigned supporters;
        
        // find white pawn in this file
        sq = mksq(x, 1);
        while (val(sq) && b.get(sq) != W_PAWN)
            inc_y(sq);
            
        if (val(sq)) {
            // count pawns supporting this pawn
            supporters = supporting_pawns(b, sq);
            if (supporters)
                score += supporters;    // if there are some, score points
            else if (get_y(sq) > 1)
                --score;                // if it has moved and is unsupported, lose points
        }
        
        // find black pawn in this file
        sq = mksq(x, 6);
        while (val(sq) && b.get(sq) != B_PAWN)
            dec_y(sq);
        
        if (val(sq)) {
            supporters = supporting_pawns(b, sq);
            if (supporters)
                score -= supporters;    // same as above, but counting the other way
            else if (get_y(sq) < 6)
                ++score;
        }
        
    }
    
    return ((float)score) / 5.0;
    
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
