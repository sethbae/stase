#include <iostream>
using std::cout;
#include <iomanip>
using std::setw;
#include <string>
using std::string;

#include "board.h"
#include "game.h"
#include "../gamestate.hpp"
#include "../eval.hpp"
#include "heur.h"
#include "heur_util.h"

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
            
            if (colour(p) == WHITE) {
                count += piece_value_millis(p);
            } else if (colour(p) == BLACK) {
                count -= piece_value_millis(p);
            } else {
                // colour can be INVALID
            }
        }
    }
    
    return count;

}

Eval heur(const Gamestate & gs, const MetricWeights * weights) {
    
    Eval ev = zero();
    
    for (const Metric * metric : ALL_METRICS) {
        float weight = (float) weights->weight(metric->id, gs.phase);
        if (weight != 0) {
            ev += (int) (metric->metric(gs) * weight);
        }
    }

    return ev + material_balance(gs);
}

Eval heur_with_description(const Gamestate & gs, const MetricWeights * weights) {

    cout << std::left << std::setprecision(3);

    cout << "Evaluating position:\n";
    board_utils::print(gs.board);

    cout << "In phase: " << name(gs.phase) << "\n";
    
    cout << "Contributions for each metric (millipawns):\n";
    
    int mat_bal = material_balance(gs);
    cout << setw(20) << "Material balance" << ": " << std::right << setw(26) << mat_bal << "\n";

    Eval ev = zero();
    for (const Metric * metric : ALL_METRICS) {
        float score = metric->metric(gs);
        int weighted_score = (int)(score * (float)weights->weight(metric->id, gs.phase));
        ev += weighted_score;
        
        cout << std::left << setw(20) << metric->name << ": "
             << std::right << setw(8) << score
             << " * "
             << setw(6) <<  weights->weight(metric->id, gs.phase)
             << " = "
             << setw(6) << weighted_score
                << "\n";
    }
    
    Eval final_ev = ev + mat_bal;
    
    cout << "\nFinal evaluation  : " << human_eval(final_ev) << "\n";
    cout << "\nFinal evaluation  : " << etos(final_ev) << "\n";
    return final_ev;

}
