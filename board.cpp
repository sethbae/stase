#include <iostream>
using std::cout;
#include <stdint.h>

/***************************************************************************
 * This file defines a board representation.
 * 
 * It defines a basic interface for interacting with a chess board:
 *  
 *   -Square: a coordinate on the board
 *      -mksq():        makes a square out of regular coordinates (a, b)
 *      -inc_row():     (in-/de-)crement a square to point further along a row/col
 *          dec_row()
 *          inc_col()
 *          dec_col()
 *      -reset_row():   return row or column to zero
 *          reset_col()
 *      -row():         return the current row or column
 *          col()
 *      -val_row():     query whether the square pointed to is on the board (or just off)
 *          val_col()
 *
 *   -Board: 64 squares and a configuration word (move, castling, ep, half moves, whole moves)
 *      -get(): return the current value of a square
 *      -set(): set the value of a square
 * 
 *
 *  Implementation details:
 *  
 *  Board:
 *      -> 4 bits per square of board
 *      -> the board struct stores a 2d array of bytes
 *      -> the config word is a 32 bits
 *      
 *  Square:
 *      -> 8 bits per square
 *      -> 4 high bits store one coordinate (0-8 inc)
 *      -> 4 low bits store another (0-8 inc)
 *      
 *  Note: To decide which half of a byte is addressed, we use even/oddness.
 *          So for a 3 bit index n, n/2 is used to get the byte (i.e. n >> 1),
 *          and then n % 2 == 0 is used to decide between high and low (i.e. n & 1).
 *      
 *****************************************************************************/

const unsigned LO4 = 15;
const unsigned HI4 = 240;
const unsigned LO3 = 7;
const unsigned HI3 = 112;

typedef uint_fast8_t Byte;
typedef uint_fast32_t Int;
typedef uint_fast8_t Square;

struct Board {

    Byte squares[8][4];
    Int conf;

    /* read 4 bits from given square address */
    int get(Square sq) {
        Byte b = squares[sq >> 4][(sq & LO3) >> 1]; // use high as 0-7, low/2 as 0-3
        return (sq & 1) ? (b & LO4) : (b >> 4); // depending on parity of low, read 4 bits
    }

    /* write 4 bits to given square address */
    void set(Square sq, Byte val) {
        
        Byte ind1 = sq >> 4;    // calculate correct indices as above
        Byte ind2 = (sq & LO3) >> 1;
        
        if (sq & 1) {
             // if odd, write to high
            squares[ind1][ind2] = (squares[ind1][ind2] & HI4) | val;
        } else {
            // if even, write to low
            squares[ind1][ind2] = (val << 4) | (squares[ind1][ind2] & LO4);
        }
        
    }

};


const Byte SHIFT_ROW = 16;
const Byte SHIFT_COL = 1;

inline Square mksq(int row, int col) {
    return (Square) ((row << 4) | col);
}

inline Square inc_row(Square s) {
    return s + SHIFT_ROW;
}

inline Square dec_row(Square s) {
    return s - SHIFT_ROW;
}

inline Square inc_col(Square s) {
    return s + SHIFT_COL;
}

inline Square dec_col(Square s) {
    return s - SHIFT_COL;
}

inline Square reset_col(Square s) {
    return s & HI4;
}

inline Square reset_row(Square s) {
    return s & LO4;
}

inline Byte row(Square s) {
    return s >> 4;
}

inline Byte col(Square s) {
    return s & LO4;
}

inline bool val_row(Square s) {
    return !(s & 128);
}

inline bool val_col(Square s) {
    return !(s & 8);
}

// print out a very basic representation of the board (grid of 64 numbers)
void pr_raw(Board b) {

    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j <=7; ++j) {
        
            int x = b.get(mksq(i, j));
            
            if (x < 10) {
                cout << " " << x << " ";
            } else {
                cout << x << " ";
            }
            
        }
        cout << "\n";
    }
    
}

/*int main() {

    Board b;
    
    int i = 0;
    for (Square s = sq(0, 0) ; row(s) < 8; s = inc_row(s)) {
        for ( ; col(s) < 8; s = inc_col(s)) {
            b.set(s, i);
        }
        ++i;
        s = reset_col(s);
    }
    
    pr_raw(b);

    return 0;
}*/
