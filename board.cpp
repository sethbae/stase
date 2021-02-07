#include <iostream>
using std::cout;
#include <stdint.h>
#include <string>
using std::string;
#include <vector>
using std::vector;

/***************************************************************************
 * This file defines a board representation and piece type (enum).
 * 
 * It defines a basic interface for interacting with a chess board:
 *  
 *   -Square: a coordinate on the board
 *      
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
 *      
 *      -b.get():       return the current piece on a square
 *      -b.set():       set the piece of a square
 * 
 *      -pr(b):         print out a human readable ascii grid chess board
 *      -pr_raw(b):     print out the numeric values in a grid
 *      -fen_to_board():convert a string in FEN to a board instance.
 *      
 *   -Piece: an enum for each piece type (W_KING, B_QUEEN, EMPTY etc).
 *          
 *      -is_white():    returns true iff the piece is white
 *      -ptoc():        returns a character depicting the piece (K, q etc)
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
 *  Piece:
 *      -> enum of type 'Byte', although only 4 (low) bits are used
 *      -> White pieces have the MSB set.
 *      -> EMPTY is white, as it goes (value 15).
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

enum Piece : Byte {
    B_KING = 0,
    B_QUEEN = 1,
    B_ROOK = 2,
    B_KNIGHT = 3,
    B_BISHOP = 4,
    B_PAWN = 5,
    
    W_KING = 8,
    W_QUEEN = 9,
    W_ROOK = 10,
    W_KNIGHT = 11,
    W_BISHOP = 12,
    W_PAWN = 13,
    
    EMPTY = 15
};

struct Board {

    Byte squares[8][4];
    Int conf;

    /* read 4 bits from given square address */
    Piece get(Square sq) {
        Byte b = squares[sq >> 4][(sq & LO3) >> 1]; // use high as 0-7, low/2 as 0-3
        return (Piece) ((sq & 1) ? (b & LO4) : (b >> 4)); // depending on parity, read 4 bits
    }

    /* write 4 bits to given square address */
    void set(Square sq, Piece val) {
        
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



inline bool is_white(Piece p) {
    return p & 8;
}

Board empty_board() {
    
    Board b;
    
    for (Square s = mksq(0, 0) ; row(s) < 8; s = inc_row(s)) {
        for ( ; col(s) < 8; s = inc_col(s)) {
            b.set(s, EMPTY);
        }
        s = reset_col(s);
    }
    
    return b;
}

Board starting_pos() {
    
    Board b = empty_board();
    
    for (Square s = mksq(1, 0); val_col(s); s = inc_col(s)) {
        b.set(s, W_PAWN);
    }
    
    for (Square s = mksq(6, 0); val_col(s); s = inc_col(s)) {
        b.set(s, B_PAWN);
    }
    
    Square s = mksq(0, 0);
    b.set(s, W_ROOK);
    
    s = inc_col(s);
    b.set(s, W_KNIGHT);
    
    s = inc_col(s);
    b.set(s, W_BISHOP);
    
    s = inc_col(s);
    b.set(s, W_QUEEN);
    
    s = inc_col(s);
    b.set(s, W_KING);
    
    s = inc_col(s);
    b.set(s, W_BISHOP);
    
    s = inc_col(s);
    b.set(s, W_KNIGHT);
    
    s = inc_col(s);
    b.set(s, W_ROOK);
    
    s = mksq(7, 0);
    b.set(s, B_ROOK);
    
    s = inc_col(s);
    b.set(s, B_KNIGHT);
    
    s = inc_col(s);
    b.set(s, B_BISHOP);
    
    s = inc_col(s);
    b.set(s, B_QUEEN);
    
    s = inc_col(s);
    b.set(s, B_KING);
    
    s = inc_col(s);
    b.set(s, B_BISHOP);
    
    s = inc_col(s);
    b.set(s, B_KNIGHT);
    
    s = inc_col(s);
    b.set(s, B_ROOK);
    
    return b;
}

char ptoc(Piece p) {
    switch (p) {
        case B_KING:    return 'k';
        case B_QUEEN:   return 'q';
        case B_ROOK:    return 'r';
        case B_BISHOP:  return 'b';
        case B_KNIGHT:  return 'n';
        case B_PAWN:    return 'p';
        case W_KING:    return 'K';
        case W_QUEEN:   return 'Q';
        case W_ROOK:    return 'R';
        case W_BISHOP:  return 'B';
        case W_KNIGHT:  return 'N';
        case W_PAWN:    return 'P';
        default:        return '*';
    }
}

Piece ctop(char c) {
    switch (c) {
        case 'K':   return W_KING;
        case 'Q':   return W_QUEEN;
        case 'R':   return W_ROOK;
        case 'B':   return W_BISHOP;
        case 'N':   return W_KNIGHT;
        case 'P':   return W_PAWN;
        case 'k':   return B_KING;
        case 'q':   return B_QUEEN;
        case 'r':   return B_ROOK;
        case 'b':   return B_BISHOP;
        case 'n':   return B_KNIGHT;
        case 'p':   return B_PAWN;
        default:    return EMPTY;
    }
}

/* print out the literal integer values of the data at each square on the board */
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

/* print out a human readable chess represenatation of the board */
void pr_indent(Board b, string indent) {
    for (int i = 7; i >= 0; --i) {
        cout << indent;
        for (int j = 0; j < 8; ++j) {
            Piece p = b.get(mksq(i, j));
            cout << ptoc(p) << " ";
        }
        cout << "\n";
    }
}

void pr(Board b) {
    pr_indent(b, "");
}

/* work in progress - doesn't read the config word */
Board fen_to_board(string fen) {
    
    Board b;
    int i = 0;  // point in string
    char c;     // char read
    
    /* read in pieces first */
    unsigned row = 7, col = 0;
    while ( i < fen.size() && (c = fen.at(i++)) != ' ') {
        
        Piece p;
        
        if (c == '/') {
            // forward slash: move on to next row
            col = 0;
            --row;
        } else if ((p = ctop(c)) != EMPTY) {
            
            // valid piece: output it
            if (row < 8 && col < 8) {
                b.set(mksq(row, col), p);
                ++col;
            } else {
                // cout << "Illegal coords: " << row << " " << col << "\n";
                return empty_board();
            }
            
        } else if ('1' <= c && c <= '8') {
            
            // legal integer: skip forward in row, printing empty
            unsigned end = col + (c - '0');
            
            while (col < end) {
                if (col < 8) {
                    b.set(mksq(row, col), EMPTY);
                    ++col;
                } else {
                    // skip overruns col, invalid fen
                    return empty_board();
                }
            }

        } else {
            // illegal character
            // cout << "Illegal char\n";
            return empty_board();
        }
        
    }
    
    // if x,y != 7,7 return failure
    return b;
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
    
    pr(starting_pos());
   
    
    vector<string> test_fens = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkb1r/pp1p1ppp/5n2/2p5/4p3/1N4P1/PPPPPPBP/RNBQK2R b KQkq - 1 5",
        "r4rk1/ppB2pp1/4p1p1/2P3q1/4Pn2/P1N2n2/2B2PPP/2R2RK1 w - - 0 24",
        "7k/1p6/p1p3p1/7p/1P2Q2P/P5P1/5r1K/5q2 w - - 4 47",
        "2r2rk1/1p2bppp/p2pbn2/q1N1p3/2P1P3/N3BP2/PP2B1PP/2RR2K1 w - - 0 17"
    };
    
    for (int i = 0; i < 5; ++i) {
        pr(fen_to_board(test_fens[i]));
        cout << "\n";
    }
    
    return 0;
}*/
