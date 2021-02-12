#include <iostream>
using std::cout;
#include <vector>
using std::vector;
#include <cstdlib>
using std::rand;

#include "board.h"
#include "helper.h"

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
        
        if (TYPE[p] == correct_types[i]) {
            ++successful;
        } else {
            ++failed;
        }
        
        if (COLOUR[p] == correct_colours[i]) {
            ++successful;
        } else {
            ++failed;
        }
    }
    
    cout << "PIECE TYPES: Passed " << successful << "/" << successful + failed << " tests\n";
    if (failed > 0) {
        cout << failed << " TESTS FAILED\n";
    }

}

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
        cout << fail << " TESTS FAILED\n";
    }
    
}

int main(void) {

    piece_types();
    setget_square();

    return 0;
}
