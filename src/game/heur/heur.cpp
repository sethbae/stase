#include <iostream>
using std::cout;
#include <iomanip>
using std::setw;
#include <string>
using std::string;

#include "game.h"
#include "board.h"
#include "heur.h"

/*
 * The order of this list is very important. It should correspond to the order
 * in which weights appear in the weights index below.
 *
 * REMEMBER TO UPDATE THE NUMBER OF METRICS USED (constant defined below)
 */
Metric* const METRICS[] = {
    &piece_activity_alpha_metric,
    &piece_activity_beta_metric,
    &piece_activity_gamma_metric,
    &open_line_control_metric,
    &centre_control_metric,
    &defended_pawns_metric,
    &development_metric,
    &isolated_pawns_metric,
    &central_pawns_metric,
    &far_advanced_pawns_metric,
    &pawns_defend_king_metric,
    &control_near_king_metric,
    &king_exposure_metric,
};
const string METRIC_NAMES[] = {
    "Alpha activity",
    "Beta activity", 
    "Gamma activity", 
    "Open line control",
    "Centre control",
    "Defended pawns",
    "Development",
    "Isolanis",
    "Central pawns",
    "Far advanced pawns",
    "Pawns defend king",
    "Control near king",
    "King exposure",
};
/*
 * The weights here are mapped by index, according to the comments and the ordering
 * of the metrics in the array above.
 * REMEMBER TO UPDATE THE NUMBER OF METRICS USED (constant defined below)
 */
const int WEIGHTS[] = {
    600,   // piece activity: alpha
    600,    // beta
    600,    // gamma
    1000,   // open line
    1500,   // centre control
    500,    // defended pawns
    500,    // development
    400,    // isolanis
    400,    // central pawns
    400,    // far advanced pawns
    750,    // pawns defending king
    750,    // control near king
    750,    // king exposure
};
const unsigned METRICS_IN_USE = 13;


// the central 16 squares
const Square CENTRAL_SQUARES[] = {
    stosq("c6"), stosq("d6"), stosq("e6"), stosq("f6"),
    stosq("c5"), stosq("d5"), stosq("e5"), stosq("f5"),
    stosq("c4"), stosq("d4"), stosq("e4"), stosq("f4"),
    stosq("c3"), stosq("d3"), stosq("e3"), stosq("f3")
};
const unsigned NUM_CENTRAL_SQUARES = 16;

// the central four squares
const Square INNER_CENTRAL_SQUARES[] = {

                 stosq("d5"), stosq("e5"),
                 stosq("d4"), stosq("e4"),
                 
};
const unsigned NUM_INNER_CENTRAL_SQUARES = 4;

// the next ring of 12 central squares
const Square OUTER_CENTRAL_SQUARES[] = {
    stosq("c6"), stosq("d6"), stosq("e6"), stosq("f6"),
    stosq("c5"),                           stosq("f5"),
    stosq("c4"),                           stosq("f4"),
    stosq("c3"), stosq("d3"), stosq("e3"), stosq("f3")
};
const unsigned NUM_OUTER_CENTRAL_SQUARES = 12;

/*
 * Returns an evaluation of the given board
 */
int material_balance(const Gamestate & gs) {

    int count = 0;
    
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

int heur(const Gamestate & gs) {
    
    int ev = zero();
    
    for (unsigned i = 0; i < METRICS_IN_USE; ++i) {
        int score = (int)(METRICS[i](gs.board) * (float)WEIGHTS[i]);
        ev += score;
    }
    
    return ev + material_balance(gs);
}

int heur_with_description(const Gamestate & gs) {

    cout << std::left << std::setprecision(3);

    cout << "Evaluating position:\n";
    pr_board(gs.board);
    
    cout << "Contributions for each metric (millipawns):\n";
    
    int mat_bal = material_balance(gs.board);
    cout << setw(20) << "Material balance" << ": " << std::right << setw(26) << mat_bal << "\n";

    int ev = zero();
    for (unsigned i = 0; i < METRICS_IN_USE; ++i) {
        float score = METRICS[i](gs.board);
        int weighted_score = (int)(score * (float)WEIGHTS[i]);
        ev += weighted_score;
        
        cout << std::left << setw(20) << METRIC_NAMES[i] << ": "
                << std::right << setw(8) << score 
                << " * " 
                << setw(6) << WEIGHTS[i] 
                << " = " 
                << setw(6) << weighted_score
                << "\n";
    }
    
    int final_ev = ev + mat_bal;
    
    cout << "\nFinal evaluation  : " << human_eval(final_ev) << "\n";
    cout << "\nFinal evaluation  : " << etos(final_ev) << "\n";
    return final_ev;

}
