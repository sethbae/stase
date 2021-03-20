#include "board.h"

#include <stdint.h>
#include <string>
#include <sstream>
using std::stringstream;
using std::string;


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
        -PIECE[p]       equals e.g. W_KING
        -TYPE[p]        equals e.g. KING
        -COLOUR[p]      equals e.g. WHITE
 *      -is_white():    returns true iff the piece is white
 *      -ptoc():        returns a character depicting the piece (K, q etc)
            ctop()
 *
 *  Implementation details:
 *  
 *  Board:
 *      -> 4 bits per square of board
 *      -> the board struct stores a 2d array of bytes
 *      -> the config word is 32 bits
 *      -> Config bits:
 *          Bit 0:          Turn colour
 *          Bit 1 - 4:      Castling Rights
 *          Bit 5:          En-passant exists
 *          Bit 6-8:        En-passant file
 *          Bit 9 - 15:     Half move counter
 *          Bit 16 - 31:    Full move counter
 *      
 *  Square:
 *      -> 8 bits per square
 *      -> 4 high bits store one coordinate (0-8 inc)
 *      -> 4 low bits store another (0-8 inc)
 *
 *  Piece:
 *      -> Each piece is a number as defined at the start of the Ptype enum. This number defines
 *          an index into the three look up tables, which record:
 *              TYPE:   the type and colour of a piece: White king
 *              PIECE:  the piece type only; king
 *              COLOUR: the colour only
 *      -> Values needed in the lookup tables (e.g. WHITE) which do not in themselves define a 
 *          piece, and hence aren't indexes into the tables, appear in the same enum without
 *          specified values.
 *      -> Any logically possible square value (e.g. empty or a chess piece) can fit in 4 bits,
 *          and can therefore be written to a square.
 *      -> White pieces have the MSB set.
 *      -> EMPTY is white, as it goes (value 14).
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
const unsigned WHITE_MASK = 1 << 0;
const unsigned CAS_WS_MASK = 1 << 1;
const unsigned CAS_WL_MASK = 1 << 2;
const unsigned CAS_BS_MASK = 1 << 3;
const unsigned CAS_BL_MASK = 1 << 4;
const unsigned EP_EX_MASK = 1 << 5;
const unsigned EP_FILE_MASK = 7 << 6;
const unsigned HALF_M_MASK = 127 << 9;
const unsigned WHOLE_M_MASK = (unsigned)(~0) << 16;

/* lookup tables for querying information about a given piece. The 4-bit piece (0-14 valid)
   is used as an index into the relevant table */

const Ptype PIECE[] = {
            B_KING, B_QUEEN, B_ROOK, B_KNIGHT, B_BISHOP, B_PAWN,        // indexes 0-5
            INVALID, INVALID,                                           // indexes 6, 7
            W_KING, W_QUEEN, W_ROOK, W_KNIGHT, W_BISHOP, W_PAWN,        // indexes 8-13
            EMPTY                                                       // index 14
      };

const Ptype TYPE[] = {
            KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN,    // indexes 0-5
            INVALID, INVALID,                           // indexes 6, 7
            KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN,    // indexes 8-13
            EMPTY                                       // index 14
      };
    
const Ptype COLOUR[] = {
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,   // indexes 0-5
            INVALID, INVALID,                           // indexes 6, 7
            WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,   // indexes 8-13
            EMPTY                                       // index 14
      };
      
Ptype piece(Piece p) { return PIECE[p]; }
Ptype type(Piece p) { return TYPE[p]; }
Ptype colour(Piece p) { return COLOUR[p]; }
bool is_white(Piece p) { return p & 8; }

/* read 4 bits from given square address */
Piece Board::get(const Square & sq) const {
    Byte b = squares[sq >> 4][(sq & LO3) >> 1]; // use high as 0-7, low/2 as 0-3
    return (Piece) ((sq & 1) ? (b & LO4) : (b >> 4)); // depending on parity, read 4 bits
}

/* write 4 bits to given square address */
void Board::set(const Square & sq, const Piece val) {
    
    Byte ind1 = sq >> 4;    // calculate correct indices as above
    Byte ind2 = (sq & LO3) >> 1;
    
    if (sq & 1) {
         // if odd, write to low
        squares[ind1][ind2] = (squares[ind1][ind2] & HI4) | val;
    } else {
        // if even, write to high
        squares[ind1][ind2] = (val << 4) | (squares[ind1][ind2] & LO4);
    }
    
}

/* get/set whole config word */
void Board::set_conf_word(Int c) { conf = c; }
Int Board::get_conf_word() const { return conf; } 

/* get/set turn (who to play) */
bool Board::get_white() const { return conf & WHITE_MASK; }
void Board::set_white(bool b) {
    if (b)
        conf |= WHITE_MASK;
    else 
        conf &= ~WHITE_MASK;
}
void Board::flip_white() { conf ^= WHITE_MASK; }
Ptype Board::colour_to_move() const {
    return (conf & WHITE_MASK) ? WHITE : BLACK;
}

/* get/set castling rights individually */
bool Board::get_cas_ws() const { return conf & CAS_WS_MASK; }    
void Board::set_cas_ws(bool b) {         
    if (b)
        conf |= CAS_WS_MASK;
    else 
        conf &= ~CAS_WS_MASK; 
}
bool Board::get_cas_wl() const { return conf & CAS_WL_MASK; }    
void Board::set_cas_wl(bool b) {         
    if (b)
        conf |= CAS_WL_MASK;
    else 
        conf &= ~CAS_WL_MASK; 
}
bool Board::get_cas_bs() const { return conf & CAS_BS_MASK; }    
void Board::set_cas_bs(bool b) {         
    if (b)
        conf |= CAS_BS_MASK;
    else 
        conf &= ~CAS_BS_MASK; 
}
bool Board::get_cas_bl() const { return conf & CAS_BL_MASK; }    
void Board::set_cas_bl(bool b) {         
    if (b)
        conf |= CAS_BL_MASK;
    else 
        conf &= ~CAS_BL_MASK; 
}

/* get/set en-passant boolean */
bool Board::get_ep_exists() const { return conf & EP_EX_MASK; }
void Board::set_ep_exists(bool b) { 
    if (b)
        conf |= EP_EX_MASK;
    else
        conf &= ~EP_EX_MASK;
}

/* read write 3 bits representing ep file */
unsigned Board::get_ep_file() const {
    return (conf & EP_FILE_MASK) >> 6;
}
void Board::set_ep_file(unsigned u) {
    conf = (conf & ~EP_FILE_MASK) | (u << 6);
}

// for convenience, return the actual square
Square Board::get_ep_sq() const {
    return (Square) (((get_white() ? 5 : 2) << 4) | get_ep_file());
}

/* read write 7 bits for the half move count */
unsigned Board::get_halfmoves() const {
    return (conf & HALF_M_MASK) >> 9;
}
void Board::set_halfmoves(unsigned u) {
    conf = (conf & ~HALF_M_MASK) | (u << 9);
}
void Board::inc_halfmoves() { conf += 1 << 9; }

/* read write 16 bits for the whole move count */
unsigned Board::get_wholemoves() const {
    return (conf & WHOLE_M_MASK) >> 16;
}
void Board::set_wholemoves(unsigned u) {
    conf = (conf & ~WHOLE_M_MASK) | (u << 16);
}
void Board::inc_wholemoves() { conf += (1 << 16); }

/* change the position of the pieces, without affecting config */
void Board::mutate(const Move m) {
    
    Ptype col = colour_to_move();
    
    // castling
    if (m.is_cas()) {   
   
        if (col == WHITE) {
            if (m.is_cas_short()) {
                set(mksq(7, 0), EMPTY);   // white short
                set(mksq(5, 0), W_ROOK);
            } else {
                set(mksq(0, 0), EMPTY);   // white long
                set(mksq(3, 0), W_ROOK);
            }
        }
        
        else {
            if (m.is_cas_short()) {
                set(mksq(7, 7), EMPTY);   // white short
                set(mksq(5, 7), B_ROOK);
            } else {
                set(mksq(0, 7), EMPTY);   // white long
                set(mksq(3, 7), B_ROOK);  
            }
        }
            
    }
    
    // en-passant
    if (m.is_ep()) {
        if (col == WHITE) {
            set(mksq(m.get_ep_file(), 4), EMPTY);
        } else {
            set(mksq(m.get_ep_file(), 3), EMPTY);
        }
    }
       
    // move piece
    Piece p = get(m.from);
    set(m.to, p);
    set(m.from, EMPTY);

    // promotion
    if (m.is_prom()) {
        Piece p = m.get_prom_piece(col);
        set(m.to, p);
    }
    
    return;
}

void Board::mutate_hard(const Move m) {
       
    // castling
    if (type(get(m.from)) == KING) {
        int delta = get_x(m.from) - get_x(m.to);
        if (delta == 2 || delta == -2) {
            
            // white short
            if (m.to == mksq(6, 0)) {
                set(mksq(7, 0), EMPTY);
                set(mksq(5, 0), W_ROOK);
            }
            
            // white long
            else if (m.to == mksq(2, 0)) {
                set(mksq(0, 0), EMPTY);
                set(mksq(3, 0), W_ROOK);
            }
            
            // black short
            else if (m.to == mksq(6, 7)) {
                set(mksq(7, 7), EMPTY);
                set(mksq(5, 7), B_ROOK);
            }
            
            // black long
            else {
                set(mksq(0, 7), EMPTY);
                set(mksq(3, 7), B_ROOK);
            }
        }
    }
    
    // en-passant
    if (type(get(m.from)) == PAWN && get_x(m.from) != get_x(m.to)) {
        if (get(m.to) == EMPTY) {
            
            // white
            if (get_y(m.to) == 5) {
                set(mksq(get_x(m.to), 4), EMPTY);
            }
            
            // black   
            else {
                set(mksq(get_x(m.to), 3), EMPTY);
            }            
        }
    }
       
    // move piece
    Piece p = get(m.from);
    set(m.to, p);
    set(m.from, EMPTY);

    // promotion
    if (type(get(m.to)) == PAWN && (get_y(m.to) == 7 || get_y(m.to) == 0)) {
        Piece p = m.get_prom_piece(colour(get(m.to)));
        set(m.to, p);
    }
    
    return;
}

/* create and return a new board, the succeeding position (config updated) */
Board Board::successor(const Move m) {
    // TODO stub
    Move m2 = m;
    m2.set_cas();
    return starting_pos();
}

Board Board::successor_hard(const Move m) {
    // TODO stub
    Move m2 = m;
    m2.set_cas();
    return starting_pos();
}

/******************This is an XOR version of get/set epfile, half and whole. Comparable speed
// read write 3 bits representing ep file 
unsigned get_ep_file() {
    return (conf & EP_FILE_MASK) >> 6;
}
void set_ep_file(unsigned u) {
    unsigned c = conf ^ (u << 6);
    conf ^= (c & EP_FILE_MASK);
}
// for convenience, return the actual square
Square get_ep_sq() {
    Square mksq(int, int);
    return mksq(get_white() ? 5 : 2, get_ep_file());
}

// read write 7 bits for the half move count 
unsigned get_halfmoves() {
    return (conf & HALF_M_MASK) >> 9;
}
void set_halfmoves(unsigned u) {
    unsigned c = conf ^ (u << 9);
    conf ^= (c & HALF_M_MASK);
}

// read write 16 bits for the half move count 
unsigned get_wholemoves() {
    return (conf & WHOLE_M_MASK) >> 16;
}
void set_wholemoves(unsigned u) {
    unsigned c = conf ^ (u << 16);
    conf ^= (c & WHOLE_M_MASK);
} */

