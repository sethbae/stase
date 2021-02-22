#include "board.h"

#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <cstdlib>
using std::rand;


/* correctness tests for all code in the board directory */

/* tests whether or not the pieces have correct types and colours associated with them */
void piece_types() {

    int successful = 0;
    int failed = 0;
    
    vector<Piece> pieces = {B_KING, B_QUEEN, B_ROOK, B_KNIGHT, B_BISHOP, B_PAWN,
                            W_KING, W_QUEEN, W_ROOK, W_KNIGHT, W_BISHOP, W_PAWN};
    vector<Ptype> correct_types = {KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN,
                                   KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN};
    vector<Ptype> correct_colours = {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,
                                     WHITE, WHITE, WHITE, WHITE, WHITE, WHITE};

    for (int i = 0; i < pieces.size(); ++i) {
        Piece p = pieces[i];
        
        if (type(p) == correct_types[i]) {
            ++successful;
        } else {
            ++failed;
        }
        
        if (colour(p) == correct_colours[i]) {
            ++successful;
        } else {
            ++failed;
        }
    }
    
    cout << "PIECE TYPES: Passed " << successful << "/" << successful + failed << " tests\n";
    if (failed > 0) {
        cout << "PIECE TYPES: " << failed << " TESTS FAILED\n";
    }

}

/* if you set to a square, do you get back what you set? */
void setget_square() {

    int succ = 0, fail = 0;

    vector<Piece> pieces = {B_KING, B_QUEEN, B_ROOK, B_KNIGHT, B_BISHOP, B_PAWN,
                            W_KING, W_QUEEN, W_ROOK, W_KNIGHT, W_BISHOP, W_PAWN};
    Board b;
    
    for (int i = 0; i < 1000; ++i) {
        
        Piece p = pieces[rand() % pieces.size()];
        Square s = mksq(rand() % 8, rand() % 8);
    
        b.set(s, p);
        
        if (b.get(s) == p) {
            ++succ;
        } else {
            ++fail;
        }
        
    }
    
    cout << "SET/GET SQUARE: Passed " << succ << "/" << succ+fail << " tests\n";
    if (fail) {
        cout << "SET/GET SQUARE: " << fail << " TESTS FAILED\n";
    }
    
}

/*do the square navigation functions work properly? */
void square_navigation(bool print) {
    
    int succ = 0, fail = 0;
    
    Square s = mksq(0, 0);
    
    /* move to h1 */
    for (int i = 0; i < 7; ++i)
        inc_x(s);
    
    if (get_x(s) == 7 && get_y(s) == 0) {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "1\n";
    }
    
    if (sqtos(s) == "h1") {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "2\n";
    }
    
    
    /* move to h8 */
    for (int i = 0; i < 7; ++i)
        inc_y(s);

    if (get_x(s) == 7 && get_y(s) == 7) {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "3\n";
    }
    
    if (sqtos(s) == "h8") {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "4\n";
    }
    
    /* move to a8 */
    for (int i = 0; i < 7; ++i)
        dec_x(s);

    if (get_x(s) == 0 && get_y(s) == 7) {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "5\n";
    }
    
    if (sqtos(s) == "a8") {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "6\n";
    }
    
    /* move to a1 */
    for (int i = 0; i < 7; ++i)
        dec_y(s);

    if (get_x(s) == 0 && get_y(s) == 0) {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "7\n";
    }
    
    if (sqtos(s) == "a1") {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "8\n";
    }
    
    /* starting from e1, take a diamond to h4, d8, a5, e1 */
    s = mksq(4, 0);
    
    for (int i = 0; i < 3; ++i)
        diag_ur(s);
    
    if (get_x(s) == 7 && get_y(s) == 3) {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "9\n";
    }
    
    if (sqtos(s) == "h4") {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "10\n";
    }
    
    /* from h4 to d8 */
    for (int i = 0; i < 4; ++i)
        diag_ul(s);
        
    if (get_x(s) == 3 && get_y(s) == 7) {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "11\n";
    }
    
    if (sqtos(s) == "d8") {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "12\n";
    }
        
    /* from d8 to a5 */
    for (int i = 0; i < 3; ++i)
        diag_dl(s);
        
    if (get_x(s) == 0 && get_y(s) == 4) {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "13\n";
    }
    
    if (sqtos(s) == "a5") {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "14\n";
    }
        
    /* and home from a5 to e1 */
    for (int i = 0; i < 4; ++i)
        diag_dr(s);
        
    if (get_x(s) == 4 && get_y(s) == 0) {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "15\n";
    }
    
    if (sqtos(s) == "e1") {
        ++succ;
    } else {
        ++fail;
        if (print)
            cout << "16\n";
    }
    
    cout << "SQUARE NAVIGATION: Passed " << succ << "/" << succ+fail << " tests\n";
    if (fail) {
        cout << "SQUARE NAVIGATION: " << fail << " TESTS FAILED\n";
    }
}

int main(void) {
    // piece_types();
    // setget_square();
    // square_navigation(false);
    Board b = starting_pos();
    b.set(mksq(4, 3), W_QUEEN);
    pr_bitmap(get_obstructed_move_map(b, mksq(4, 3)));
    return 0;
}
