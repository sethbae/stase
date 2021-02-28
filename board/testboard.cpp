#include "board.h"

#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <string>
using std::string;
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

    for (unsigned i = 0; i < pieces.size(); ++i) {
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

void test_in_check() {

    vector<string> test_fens = {"3k4/8/8/8/8/8/4p3/3K4 w K - 0 1",  // pawn checking
                                "3k4/2P5/8/8/8/8/8/3K4 b K - 0 1",
                                "3k4/3P4/8/8/8/8/3p4/3K4 w - - 0 1", // pawn not checking
                                "3k4/3P4/8/8/8/8/3p4/3K4 b - - 0 1",
                                "3k4/8/8/8/8/5b2/8/3K4 w K - 0 1",  // bishop checking
                                "7k/8/5B2/8/8/8/8/3K4 b K - 0 1",
                                "1k6/8/8/8/8/5b2/4b3/6K1 w - - 0 1", // bishop not checking
                                "1k6/3B4/2B5/8/8/8/8/6K1 b - - 0 1",
                                "1k6/8/4n3/8/3K4/8/8/8 w K - 0 1",  // knight checking
                                "1k6/8/2N5/8/3K4/8/8/8 b K - 0 1",
                                "1k6/8/3n4/8/3Kn3/8/3n4/8 w - - 0 1", // knight not checking
                                "1k6/2N5/1N6/3N4/3K4/8/8/8 b - - 0 1",
                                "1k1r4/8/8/8/3K4/8/8/8 w K - 0 1",  // rook checking
                                "1k1R4/8/8/8/3K4/8/8/8 b K - 0 1",
                                "1kr1r3/8/8/8/3K4/7r/8/8 w - - 0 1", // rook not checking
                                "1k6/2RR4/8/8/3K4/8/8/7R b - - 0 1",
                                "1k6/8/5q2/8/3K4/8/8/8 w K - 0 1",  // queen checking
                                "1k6/8/3Q4/8/3K4/8/8/8 b K - 0 1",
                                "1k4q1/8/2q5/5q2/3K4/1q6/8/8 w - - 0 1", // queen not checking
                                "1k6/3Q4/Q7/3Q4/3K4/8/2Q5/2Q5 b - - 0 1",
                                "1k6/8/8/8/2n5/q2r4/2B5/1KR5 w - - 0 1", // medley no check
                                "1k6/2rQN3/B1QB4/1P6/2n5/q2r4/RP6/1K6 b - - 0 1",
                                "1k6/2rQN3/B1QB4/1P6/2n5/q2r4/RP6/1K5q w K - 0 1", //medley check
                                "1k6/P1rQN3/B1QB4/1P6/2n5/q2r4/RP6/1K5q w K - 0 1"
                                };

    int succ = 0, fail = 0;

    for (string fen : test_fens) {
        
        Board b = fen_to_board(fen);
        
        bool check = in_check(b);
        
        if (check != b.get_cas_ws()) {
            cout << "TEST FAILED:\n";
            pr_board(b);
            cout << "In check returned: " << (check ? "true\n" : "false\n");
            ++fail;
        } else {
            ++succ;
        }   
        
    }
    
    cout << "IN CHECK: Passed " << succ << "/" << test_fens.size() << " tests\n";
    if (fail) {
        cout << "IN CHECK: " << fail << " TESTS FAILED\n";
    }

}

int main(void) {
    piece_types();
    setget_square();
    square_navigation(false);
    test_in_check();
    
    //Board b = starting_pos();
    //b.set(mksq(4, 3), W_QUEEN);
    //pr_bitmap(get_obstructed_move_map(b, mksq(4, 3)));
    return 0;
}
