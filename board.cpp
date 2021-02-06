//definition of a board

#include <iostream>
using std::cout;
#include <stdint.h>

/* I use the convention that one byte [xxxx-xxxx] stores the data at the even index in high bits,
   the odd in low bits */
   

// masks for bit reading
const unsigned LO4_MASK = 15;
const unsigned HI4_MASK = 240;
const unsigned LO3_MASK = 7;
const unsigned HI3_MASK = 112;

// types we use for small numbers (8 or 32 bit)
typedef uint_fast8_t Num;
typedef uint_fast32_t Int;

// type definition for a single square address
typedef uint_fast8_t Square;

// numbers used to increment either the row or the col of a square
const Num ONE_ROW = 8;
const Num ONE_COL = 1;

// type definition for the board
typedef struct Board {

    Num squares[8][4];
    Int conf;

    // read 4 bits corresponding to a square address
    int get(Square sq) {
//        Num ind = ((sq & HI3_MASK) >> 2) | ((sq & LO3_MASK) >> 1);
        Num byte = squares[sq >> 4][(sq & LO3_MASK) >> 1];
        return (sq & 1) ? (byte & LO4_MASK) : (byte >> 4);
    }
    
    // set a square address to the value given
    void set(Square sq, Num val) {
        //Num ind = ((sq & HI3_MASK) >> 2) | ((sq & LO3_MASK) >> 1);
        if (sq & 1) {
            squares[sq >> 4][(sq & LO3_MASK) >> 1] = 
                (squares[sq >> 4][(sq & LO3_MASK) >> 1] & HI4_MASK) | val;            
        } else {
            squares[sq >> 4][(sq & LO3_MASK) >> 1] = 
                (val << 4) | (squares[sq >> 4][(sq & LO3_MASK) >> 1] & LO4_MASK);
        }
    }

} Board;

// make a square out of human coordinates
inline Square sq(int row, int col) {
    return (Square) ((row << 4) | col);
}

// increment or decrement along the row or column
inline Square inc_row(Square s) {
    return s + ONE_ROW;
}

inline Square dec_row(Square s) {
    return s - ONE_ROW;
}

inline Square inc_col(Square s) {
    return s + ONE_COL;
}

inline Square dec_col(Square s) {
    return s - ONE_COL;
}

// read the row or column of a square
inline Num row(Square sq) {
    return sq >> 4;
}

inline Num col(Square sq) {
    return sq & LO4_MASK;
}

// print out a very basic representation of the board (grid of 64 numbers)
void pr_raw(Board b) {

    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j <=7; ++j) {
        
            int x = b.get(sq(i, j));
            
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

    Square s = sq(0, 0);
    cout << (int) s << "\n";
    s = sq(1, 0);
    cout << (int) s << "\n";
    s = sq(0, 1);
    cout << (int) s << "\n";
    s = sq(7, 7);
    cout << (int) s << "\n";
    s = sq(7, 6);
    cout << (int) s << "\n";
    s = sq(6, 7);
    cout << (int) s << "\n";
    
    cout << (int) row(sq(6, 3)) << "\n";
    cout << (int) row(sq(3, 4)) << "\n";
    
    
    Board b;
    
    Square current = sq(0, 0);
    for ( ; row(current) < 8; current = inc_row(current)) {
        b.set(current, 6);
    }
    for ( ; col(current) < 6; current = inc_col(current)) {
        b.set(current, 5);
    }
    
    pr_raw(b);

    return 0;
}*/
