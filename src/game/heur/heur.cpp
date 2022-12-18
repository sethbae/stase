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
            
            if (colour(p) == WHITE)
                count += piece_value_millis(p);
            else if (colour(p) == BLACK)
                count -= piece_value_millis(p);
            // colour can be INVALID
        
        }
    }
    
    return count;

}

Eval heur(const Gamestate & gs) {
    
    Eval ev = zero();
    
    for (unsigned i = 0; i < ALL_METRICS.size(); ++i) {
        int score = (int)(ALL_METRICS[i]->metric(gs) * (float)ALL_METRICS[i]->weights[gs.phase]);
        ev += score;
    }
    
    return ev + material_balance(gs);
}

Eval heur_with_description(const Gamestate & gs) {

    cout << std::left << std::setprecision(3);

    cout << "Evaluating position:\n";
    pr_board(gs.board);

    cout << "In phase: " << name(gs.phase) << "\n";
    
    cout << "Contributions for each metric (millipawns):\n";
    
    int mat_bal = material_balance(gs);
    cout << setw(20) << "Material balance" << ": " << std::right << setw(26) << mat_bal << "\n";

    Eval ev = zero();
    for (unsigned i = 0; i < ALL_METRICS.size(); ++i) {
        float score = ALL_METRICS[i]->metric(gs);
        int weighted_score = (int)(score * (float)ALL_METRICS[i]->weights[gs.phase]);
        ev += weighted_score;
        
        cout << std::left << setw(20) << ALL_METRICS[i]->name << ": "
                << std::right << setw(8) << score 
                << " * " 
                << setw(6) << ALL_METRICS[i]->weights[gs.phase]
                << " = " 
                << setw(6) << weighted_score
                << "\n";
    }
    
    Eval final_ev = ev + mat_bal;
    
    cout << "\nFinal evaluation  : " << human_eval(final_ev) << "\n";
    cout << "\nFinal evaluation  : " << etos(final_ev) << "\n";
    return final_ev;

}
