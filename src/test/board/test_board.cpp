#include "board.h"

#include <iostream>
using std::cout;
using std::cin;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <cstdlib>
using std::rand;


/* correctness tests for all code in the board directory */

void print_results(int succ, int fail, string test_name) {
    cout << test_name;
    for (int i = test_name.size(); i < 30; ++i)
        cout << " ";
        
    cout << "Passed " << succ << "/" << succ + fail << " tests\n";
    
    if (fail == 1) {
        cout << "*****ONE TEST FAILED*****\n";
    } else if (fail > 1) {
        cout << "*****SOME TESTS FAILED*****\n";
    }
}

/* tests whether or not the pieces have correct types and colours associated with them */
bool piece_types() {

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
    
    
    print_results(successful, failed, "PIECE_TYPES");

    return failed == 0;
}

/* if you set to a square, do you get back what you set? */
bool setget_square() {

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
    
    print_results(succ, fail, "SET/GET SQUARE");
    return fail == 0;
}

/*do the square navigation functions work properly? */
bool square_navigation() {
    
    int succ = 0, fail = 0;
    bool print = false;
    
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
    
        print_results(succ, fail, "SQUARE NAVIGATION");
        return fail == 0;
}

bool rw_fens() {
    
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
    
    for (const string & s : test_fens) {
        
        if (board_to_fen(fen_to_board(s)) == s)
            ++succ;
        else
            ++fail;   
        
    }
    
    print_results(succ, fail, "RW_FEN");
    return fail == 0;
}

bool test_in_check() {

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
        
        bool check = in_check_hard(b);
        
        if (check != b.get_cas_ws()) {
            cout << "TEST FAILED:\n";
            pr_board(b);
            cout << "In check returned: " << (check ? "true\n" : "false\n");
            ++fail;
        } else {
            ++succ;
        }   
        
    }
    
    print_results(succ, fail, "IN CHECK");
    return fail == 0;
    
}

bool test_castling() {

    //"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
    //"r3k2r/8/8/8/8/8/8/R3KN1R w KQkq - 0 1"
    //"r3k2r/8/8/8/8/8/8/R2bK2R w KQkq - 0 1"
    //"r3kN1r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
    //"rq2k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
    //"rq2k2r/b7/8/8/8/8/8/R3K2R w KQkq - 0 1"
    

    return false;
}

bool test_mutate_hard() {

    Board b;
    Move m;
    int succ = 0, fail = 0;

    // queen
    
    // Qd3-h7
    b = fen_to_board("8/3q4/8/8/8/3Q4/8/8 w - - 0 1");
    m = {mksq(3, 2), mksq(7, 6), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/3q3Q/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
    
    // Qd3xd7
    b = fen_to_board("8/3q4/8/8/8/3Q4/8/8 w - - 0 1");
    m = {mksq(3, 2), mksq(3, 6), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/3Q4/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;

    // rook    
    
    // Ra7-a2
    b = fen_to_board("8/r2Q4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(0, 6), mksq(0, 1), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/3Q4/8/8/8/8/r7/8 w - - 0 1")
        ++succ;
    else
        ++fail;
    
    // Ra7xd7
    b = fen_to_board("8/r2Q4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(0, 6), mksq(3, 6), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/3r4/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // bishop
    
    // Bg4-e2
    b = fen_to_board("8/3r4/8/8/6B1/8/8/8 w - - 0 1");
    m = {mksq(6, 3), mksq(4, 1), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/3r4/8/8/8/8/4B3/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // Bg4xd7
    b = fen_to_board("8/3r4/8/8/6B1/8/8/8 w - - 0 1");
    m = {mksq(6, 3), mksq(3, 6), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/3B4/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // knight
    
    // Nf8-g6
    b = fen_to_board("5n2/3B4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(5, 7), mksq(6, 5), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/3B4/6n1/8/8/8/8/8 w - - 0 1")
       ++succ;
    else
       ++fail;
        
    // Nf8xd7
    b = fen_to_board("5n2/3B4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(5, 7), mksq(3, 6), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/3n4/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // king (regular)
     
    // Ke1xe2
    b = fen_to_board("8/8/8/q7/8/8/4bn2/4K3 w - - 0 1");
    m = {mksq(4, 0), mksq(4, 1), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/8/8/q7/8/8/4Kn2/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // Ke1xf2
    b = fen_to_board("8/8/8/q7/8/8/4bn2/4K3 w - - 0 1");
    m = {mksq(4, 0), mksq(5, 1), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "8/8/8/q7/8/8/4bK2/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // king (castle)
    
    // ws
    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
    m = {mksq(4, 0), mksq(6, 0), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "r3k2r/8/8/8/8/8/8/R4RK1 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // wl
    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
    m = {mksq(4, 0), mksq(2, 0), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) ==  "r3k2r/8/8/8/8/8/8/2KR3R w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // bs
    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
    m = {mksq(4, 7), mksq(6, 7), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) ==  "r4rk1/8/8/8/8/8/8/R3K2R w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // bl
    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
    m = {mksq(4, 7), mksq(2, 7), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) ==  "2kr3r/8/8/8/8/8/8/R3K2R w - - 0 1")
        ++succ;
    else
        ++fail;
    
    // pawn (regular)
    
    // e5-e4
    b = fen_to_board("r3k2r/8/8/4p3/8/8/8/R3K2R w - - 0 1");
    m = {mksq(4, 4), mksq(4, 3), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) ==  "r3k2r/8/8/8/4p3/8/8/R3K2R w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // pawn (e-p)
    
    // f5xe6
    b = fen_to_board("r3k2r/8/8/4pP2/8/8/8/R3K2R w - - 0 1");
    m = {mksq(5, 4), mksq(4, 5), 0};
    b.mutate_hard(m);
    if (board_to_fen(b) == "r3k2r/8/4P3/8/8/8/8/R3K2R w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // pawn (promotion)
    
    // d8=Q
    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(3, 6), mksq(3, 7), 0};
    m.set_prom_piece(QUEEN);
    b.mutate_hard(m);
    if (board_to_fen(b) == "3Q4/8/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // d8=R
    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(3, 6), mksq(3, 7), 0};
    m.set_prom_piece(ROOK);
    b.mutate_hard(m);
    if (board_to_fen(b) == "3R4/8/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // d8=N
    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(3, 6), mksq(3, 7), 0};
    m.set_prom_piece(KNIGHT);
    b.mutate_hard(m);
    if (board_to_fen(b) == "3N4/8/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // d8=B
    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(3, 6), mksq(3, 7), 0};
    m.set_prom_piece(BISHOP);
    b.mutate_hard(m);
    if (board_to_fen(b) == "3B4/8/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;

    
    print_results(succ, fail, "MUTATE HARD");
    return fail == 0;
}

bool test_mutate() {

    Board b;
    Move m;
    int succ = 0, fail = 0;

    // queen
    
    // Qd3-h7
    b = fen_to_board("8/3q4/8/8/8/3Q4/8/8 w - - 0 1");
    m = {mksq(3, 2), mksq(7, 6), 0};
    b.mutate(m);
    if (board_to_fen(b) == "8/3q3Q/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
    
    // Qd3xd7
    b = fen_to_board("8/3q4/8/8/8/3Q4/8/8 w - - 0 1");
    m = {mksq(3, 2), mksq(3, 6), 0};
    m.set_cap();
    b.mutate(m);
    if (board_to_fen(b) == "8/3Q4/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;

    // rook    
    
    // Ra7-a2
    b = fen_to_board("8/r2Q4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(0, 6), mksq(0, 1), 0};
    b.mutate(m);
    if (board_to_fen(b) == "8/3Q4/8/8/8/8/r7/8 w - - 0 1")
        ++succ;
    else
        ++fail;
    
    // Ra7xd7
    b = fen_to_board("8/r2Q4/8/8/8/8/8/8 b - - 0 1");
    m = {mksq(0, 6), mksq(3, 6), 0};
    m.set_cap();
    b.mutate(m);
    if (board_to_fen(b) == "8/3r4/8/8/8/8/8/8 b - - 0 1")
        ++succ;
    else
        ++fail;
        
    // bishop
    
    // Bg4-e2
    b = fen_to_board("8/3r4/8/8/6B1/8/8/8 w - - 0 1");
    m = {mksq(6, 3), mksq(4, 1), 0};
    b.mutate(m);
    if (board_to_fen(b) == "8/3r4/8/8/8/8/4B3/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // Bg4xd7
    b = fen_to_board("8/3r4/8/8/6B1/8/8/8 w - - 0 1");
    m = {mksq(6, 3), mksq(3, 6), 0};
    m.set_cap();
    b.mutate(m);
    if (board_to_fen(b) == "8/3B4/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // knight
    
    // Nf8-g6
    b = fen_to_board("5n2/3B4/8/8/8/8/8/8 b - - 0 1");
    m = {mksq(5, 7), mksq(6, 5), 0};
    b.mutate(m);
    if (board_to_fen(b) == "8/3B4/6n1/8/8/8/8/8 b - - 0 1")
       ++succ;
    else
       ++fail;
        
    // Nf8xd7
    b = fen_to_board("5n2/3B4/8/8/8/8/8/8 b - - 0 1");
    m = {mksq(5, 7), mksq(3, 6), 0};
    m.set_cap();
    b.mutate(m);
    if (board_to_fen(b) == "8/3n4/8/8/8/8/8/8 b - - 0 1")
        ++succ;
    else
        ++fail;
        
    // king (regular)
     
    // Ke1xe2
    b = fen_to_board("8/8/8/q7/8/8/4bn2/4K3 w - - 0 1");
    m = {mksq(4, 0), mksq(4, 1), 0};
    m.set_cap();
    b.mutate(m);
    if (board_to_fen(b) == "8/8/8/q7/8/8/4Kn2/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // Ke1xf2
    b = fen_to_board("8/8/8/q7/8/8/4bn2/4K3 w - - 0 1");
    m = {mksq(4, 0), mksq(5, 1), 0};
    m.set_cap();
    b.mutate(m);
    if (board_to_fen(b) == "8/8/8/q7/8/8/4bK2/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // king (castle)
    
    // ws
    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
    m = {mksq(4, 0), mksq(6, 0), 0};
    m.set_cas();
    m.set_cas_short();
    b.mutate(m);
    if (board_to_fen(b) == "r3k2r/8/8/8/8/8/8/R4RK1 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // wl
    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1");
    m = {mksq(4, 0), mksq(2, 0), 0};
    m.set_cas();
    m.unset_cas_short();
    b.mutate(m);
    if (board_to_fen(b) ==  "r3k2r/8/8/8/8/8/8/2KR3R w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // bs
    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R b - - 0 1");
    m = {mksq(4, 7), mksq(6, 7), 0};
    m.set_cas();
    m.set_cas_short();
    b.mutate(m);
    if (board_to_fen(b) ==  "r4rk1/8/8/8/8/8/8/R3K2R b - - 0 1")
        ++succ;
    else
        ++fail;
        
    // bl
    b = fen_to_board("r3k2r/8/8/8/8/8/8/R3K2R b - - 0 1");
    m = {mksq(4, 7), mksq(2, 7), 0};
    m.set_cas();
    m.unset_cas_short();
    b.mutate(m);
    if (board_to_fen(b) ==  "2kr3r/8/8/8/8/8/8/R3K2R b - - 0 1")
        ++succ;
    else
        ++fail;
    
    // pawn (regular)
    
    // e5-e4
    b = fen_to_board("r3k2r/8/8/4p3/8/8/8/R3K2R b - - 0 1");
    m = {mksq(4, 4), mksq(4, 3), 0};
    b.mutate(m);
    if (board_to_fen(b) ==  "r3k2r/8/8/8/4p3/8/8/R3K2R b - - 0 1")
        ++succ;
    else
        ++fail;
        
    // pawn (e-p)
    
    // f5xe6
    b = fen_to_board("r3k2r/8/8/4pP2/8/8/8/R3K2R w - - 0 1");
    m = {mksq(5, 4), mksq(4, 5), 0};
    m.set_cap();
    m.set_ep();
    b.mutate(m);
    if (board_to_fen(b) == "r3k2r/8/4P3/8/8/8/8/R3K2R w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // pawn (promotion)
    
    // d8=Q
    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(3, 6), mksq(3, 7), 0};
    m.set_prom_piece(QUEEN);
    m.set_prom();
    b.mutate(m);
    if (board_to_fen(b) == "3Q4/8/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // d8=R
    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(3, 6), mksq(3, 7), 0};
    m.set_prom_piece(ROOK);
    m.set_prom();
    b.mutate(m);
    if (board_to_fen(b) == "3R4/8/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // d8=N
    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(3, 6), mksq(3, 7), 0};
    m.set_prom_piece(KNIGHT);
    m.set_prom();
    b.mutate(m);
    if (board_to_fen(b) == "3N4/8/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;
        
    // d8=B
    b = fen_to_board("8/3P4/8/8/8/8/8/8 w - - 0 1");
    m = {mksq(3, 6), mksq(3, 7), 0};
    m.set_prom_piece(BISHOP);
    m.set_prom();
    b.mutate(m);
    if (board_to_fen(b) == "3B4/8/8/8/8/8/8/8 w - - 0 1")
        ++succ;
    else
        ++fail;

    print_results(succ, fail, "MUTATE");
    return fail == 0;
}

/****************************************************************
 * Methods below here are not currently used but should be
 * refactored or deleted (BRD-10)
 ***************************************************************/

void starting_pos_test() {
    Board b = starting_pos();

    int sum = 0;
    for (int i = 0; i < 64; ++i) {
        sum += b.get(i);
    }

    cout << "Starting position test:\n";
    cout << "\tSum: " << sum << "\n";
    cout << "\tArrangement: \n\n";
    pr_board_conf(b, "\t");

    cout << "\n";

}

void board_from_fen_test() {

    cout << "Board from FEN test:\n\n";

    vector<string> test_fens = {
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "rnbqkb1r/pp1p1ppp/5n2/2p5/4p3/1N4P1/PPPPPPBP/RNBQK2R b KQkq - 1 5",
            "r4rk1/ppB2pp1/4p1p1/2P3q1/4Pn2/P1N2n2/2B2PPP/2R2RK1 w - - 0 24",
            "7k/1p6/p1p3p1/7p/1P2Q2P/P5P1/5r1K/5q2 w - h6 4 47",
            "2r2rk1/1p2bppp/p2pbn2/q1N1p3/2P1P3/N3BP2/PP2B1PP/2RR2K1 w - - 0 17"
    };

    for (int i = 0; i < 5; ++i) {
        cout << i << ".      Required:                    " << test_fens[i] << "\n";
        Board b = fen_to_board(test_fens[i]);
        //cout << "WS: " << b.get_cas_ws() << "\n";
        pr_board_conf(b, "\t");
        cout << "\n";
    }

}

void fen_from_board_test() {

    cout << "FEN from Board test:\n";

    vector<string> test_fens = {
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "rnbqkb1r/pp1p1ppp/5n2/2p5/4p3/1N4P1/PPPPPPBP/RNBQK2R b KQkq - 1 5",
            "r4rk1/ppB2pp1/4p1p1/2P3q1/4Pn2/P1N2n2/2B2PPP/2R2RK1 w - - 0 24",
            "7k/1p6/p1p3p1/7p/1P2Q2P/P5P1/5r1K/5q2 w - h6 4 47",
            "2r2rk1/1p2bppp/p2pbn2/q1N1p3/2P1P3/N3BP2/PP2B1PP/2RR2K1 w - - 0 17",
            "rnbqkbnr/pppp1ppp/8/4p3/8/4P3/PPPPKPPP/RNBQ1BNR b kq - 1 2"
    };

    for (int i = 0; i < (int) test_fens.size(); ++i) {
        Board b = fen_to_board(test_fens[i]);

        if (board_to_fen(b) == test_fens[i]) {
            cout << "\t" << i + 1 << ". Passed.\n";
        } else {
            cout << "\t" << i + 1 << ". Failed.\n";
        }

        // cout << "\tFEN: " << test_fens[i] << "\n";
        pr_board_conf(b, "\t");
        cout << "\n";

    }

}

bool test_legal_moves() {
    return false;
}

/*void gamestate_generation() {
    Board b = starting_pos();
    Gamestate g(b);

    cout << "Board:\n";
    pr_board(b);

    cout << "\nFriendly attack:\n";
    pr_bitmap(g.fattack);

    cout << "\nEnemy attack:\n";
    pr_bitmap(g.eattack);

    cout << "\nFriendly pieces:\n";
    pr_bitmap(g.foccupy);

    cout << "\nEnemy pieces:\n";
    pr_bitmap(g.eoccupy);

    cout << "\nOccupany:\n";
    pr_bitmap(g.occupancy);

    cout << "\nVacancy:\n";
    pr_bitmap(g.vacancy);

    cout << "\nKings:\n";
    pr_bitmap(g.kings);

    cout << "\nQueens:\n";
    pr_bitmap(g.queens);

    cout << "\nBishops:\n";
    pr_bitmap(g.bishops);

    cout << "\nKnights:\n";
    pr_bitmap(g.knights);

    cout << "\nRooks:\n";
    pr_bitmap(g.rooks);

    cout << "\nPawns:\n";
    pr_bitmap(g.pawns);

}*/

int test_board(void) {

    vector<bool (*)(void)> tests = {
        &piece_types,
        &setget_square,
        &square_navigation,
        &rw_fens,
        &test_in_check,
        &test_mutate_hard,
        &test_mutate
    };

    for (auto test : tests) {
        if (!(*test)())
            exit(1);
    }

    return 0;
}
