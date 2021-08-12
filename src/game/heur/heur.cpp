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
    &piece_activity_alpha,
    &piece_activity_beta,
    &piece_activity_gamma,
    &open_line_control,
    &centre_control,
    &defended_pawns,
    &development,
    &isolated_pawns,
    &central_pawns,
    &far_advanced_pawns
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
    "Far advanced pawns"
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
};
const unsigned METRICS_IN_USE = 10;


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
    
    int ev = 0;
    
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

    int ev = 0;
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
    return final_ev;

}

// returns the number of pawns which directly defend the given pawn (square),
// relying on the colour of the piece on the square given
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

        }
        
        // find black pawn in this file
        sq = mksq(x, 6);
        while (val(sq) && b.get(sq) != B_PAWN)
            dec_y(sq);
        
        if (val(sq)) {
            supporters = supporting_pawns(b, sq);
            if (supporters)
                score -= supporters;    // same as above, but counting the other way
        }
        
    }
    
    return ((float)score) / 5.0;
    
}

float isolated_pawns(const Board & b) {
    
    int score = 0;
    
    // TODO this is unlikely to be efficient
    
    // true indicates that black/white has a pawn in that file
    bool wpawns[8] = { false, false, false, false, false, false, false, false };
    bool bpawns[8] = { false, false, false, false, false, false, false, false };
    
    // compute the above arrays
    for (unsigned x = 0; x < 8; ++x) {
        for (unsigned y = 0; y < 7; ++y) {
            if (b.get(mksq(x, y)) == W_PAWN) {
                wpawns[x] = true;
            } else if (b.get(mksq(x, y)) == B_PAWN) {
                bpawns[x] = true;
            }   
        }
    }
       
    // find files which are isolated for white (decrementing)
    if (wpawns[0] && !wpawns[1]) {
        --score;
    }
    for (unsigned x = 1; x < 7; ++x) {
        if (!wpawns[x - 1] && wpawns[x] && !wpawns[x + 1]) {
            --score;
        }
    }
    if (!wpawns[6] && wpawns[7]) {
        --score;
    }
    
    // find files which are isolated for black (incrementing)
    if (bpawns[0] && !bpawns[1]) {
        ++score;
    }
    for (unsigned x = 1; x < 7; ++x) {
        if (!bpawns[x - 1] && bpawns[x] && !bpawns[x + 1]) {
            ++score;
        }
    }
    if (!bpawns[6] && bpawns[7]) {
        ++score;
    }
    
    return ((float) score) / 3.0f;
    
}

float central_pawns(const Board & b) {
    
   int score = 0;
    
    // true indicates that black/white has a pawn in that file
    bool wpawns[8] = { false, false, false, false, false, false, false, false };
    bool bpawns[8] = { false, false, false, false, false, false, false, false };
    
    // compute the above arrays
    for (unsigned x = 0; x < 8; ++x) {
        for (unsigned y = 0; y < 7; ++y) {
            if (b.get(mksq(x, y)) == W_PAWN) {
                wpawns[x] = true;
            } else if (b.get(mksq(x, y)) == B_PAWN) {
                bpawns[x] = true;
            }   
        }
    }
    
    if (wpawns[2])
        score += 1;
    if (wpawns[3])
        score += 2;
    if (wpawns[4])
        score += 2;
    if (wpawns[5])
        score += 1;
        
    if (bpawns[2])
        score -= 1;
    if (bpawns[3])
        score -= 2;
    if (bpawns[4])
        score -= 2;
    if (bpawns[5])
        score -= 1;
        
    return ((float) score) / 5.0f;
    
}

float far_advanced_pawns(const Board & b) {
    
    int score = 0;
    
    int value[8] = { 0, 4, 2, 1, 1, 2, 4, 0 };

    for (unsigned y = 6; y > 0; --y) {
        for (unsigned x = 0; x < 8; ++x) {
            
            Piece p = b.get(mksq(x, y));
            
            if (y > 3 && p == W_PAWN) {
                score += value[y];
            }
            if (y <= 3 && p == B_PAWN) {
                score -= value[y];
            }
            
        }
    }
    
    return ((float) score) / 10.0f;
   
}

float king_safety(const Board & b) {
    
    // TODO
    
    // pawns in front of king
    
    // control of squares in front of king
    
    // prominent diagonals + files
    
    // attacking pieces vs defending pieces
    
    b.get(mksq(0, 0));
    return 0.0f;

}

float development(const Board & b) {

    int count = 0;
    
    const Piece BACK_RANK[] = {
        ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
    };
    
    for (Square sq = stosq("a8"); val(sq); inc_x(sq)) {
        if (type(b.get(sq)) == BACK_RANK[get_x(sq)]) {
            ++count;
        }
    }

    for (Square sq = stosq("a1"); val(sq); inc_x(sq)) {
        if (type(b.get(sq)) == BACK_RANK[get_x(sq)]) {
            --count;
        }
    }
    
    return ((float)count) / 4.0f;

}

/*
float space(const Board & b) {              // forget for now
    return 0;
}
*/
