#ifndef BOARD_H
#define BOARD_H

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
 *      -is_white():    returns true iff the piece is white
 *      -ptoc():        returns a character depicting the piece (K, q etc)
 *
 *  Implementation details:
 *  
 *  Board:
 *      -> 4 bits per square of board
 *      -> the board struct stores a 2d array of bytes
 *      -> the config word is a 32 bits
 *      - Config bits:
 *          Bit 0:          Turn colour
 *          Bit 1 - 4:      Castling Rights
 *          Bit 5 - 8:      En passant
 *          Bit 9 - 15:     Half move counter
 *          Bit 16 - 31:    Full move counter
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

enum Ptype {

    /* defined values: so that we can translate both ways */
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
    
    EMPTY = 14,
    
    /* other values which appear in the lookup tables */
    KING,
    QUEEN,
    ROOK,
    KNIGHT,
    BISHOP,
    PAWN,
    
    BLACK,
    WHITE,
    
    INVALID
};

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

typedef Byte Piece;

struct Conf {
    bool w : 1;
    bool K : 1;
    bool Q : 1;
    bool k : 1;
    bool q : 1;
    bool ep : 1;
    unsigned epfile : 3;
    unsigned half_moves : 6;
    unsigned whole_moves : 17;
};

const unsigned WHITE_MASK = 1 << 0;
const unsigned CAS_WS_MASK = 1 << 1;
const unsigned CAS_WL_MASK = 1 << 2;
const unsigned CAS_BS_MASK = 1 << 3;
const unsigned CAS_BL_MASK = 1 << 4;
const unsigned EP_EX_MASK = 1 << 5;
const unsigned EP_FILE_MASK = 7 << 6;
const unsigned HALF_M_MASK = 127 << 9;
const unsigned WHOLE_M_MASK = (~0) << 16;


struct Board {

    // std::array<std::array<Byte, 4>, 8> squares;
    Byte squares[8][4];
    Int conf;

    /* read 4 bits from given square address */
    Piece get(Square sq) const {
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
    
    /* get/set turn (who to play) */
    inline bool get_white() const { return conf & WHITE_MASK; }
    inline void set_white(bool b) {
        if (b)
            conf |= WHITE_MASK;
        else 
            conf &= ~WHITE_MASK;
    }
    inline void flip_white() { conf ^= WHITE_MASK; }
    
    /* get/set castling rights individually */
    inline bool get_cas_ws() const { return conf & CAS_WS_MASK; }    
    inline void set_cas_ws(bool b) {         
        if (b)
            conf |= CAS_WS_MASK;
        else 
            conf &= ~CAS_WS_MASK; 
    }
    inline bool get_cas_wl() const { return conf & CAS_WL_MASK; }    
    inline void set_cas_wl(bool b) {         
        if (b)
            conf |= CAS_WL_MASK;
        else 
            conf &= ~CAS_WL_MASK; 
    }
    inline bool get_cas_bs() const { return conf & CAS_BS_MASK; }    
    inline void set_cas_bs(bool b) {         
        if (b)
            conf |= CAS_BS_MASK;
        else 
            conf &= ~CAS_BS_MASK; 
    }
    inline bool get_cas_bl() const { return conf & CAS_BL_MASK; }    
    inline void set_cas_bl(bool b) {         
        if (b)
            conf |= CAS_BL_MASK;
        else 
            conf &= ~CAS_BL_MASK; 
    }
    
    /* get/set en-passant boolean */
    inline bool get_ep_exists() const { return conf & EP_EX_MASK; }
    inline void set_ep_exists(bool b) { 
        if (b)
            conf |= EP_EX_MASK;
        else
            conf &= ~EP_EX_MASK;
    }
    
    /* read write 3 bits representing ep file */
    inline unsigned get_ep_file() const {
        return (conf & EP_FILE_MASK) >> 6;
    }
    inline void set_ep_file(unsigned u) {
        conf = (conf & ~EP_FILE_MASK) | (u << 6);
    }

    // for convenience, return the actual square
    inline Square get_ep_sq() const {
        return (Square) (((get_white() ? 5 : 2) << 4) | get_ep_file());
    }
    
    /* read write 7 bits for the half move count */
    inline unsigned get_halfmoves() const {
        return (conf & HALF_M_MASK) >> 9;
    }
    inline void set_halfmoves(unsigned u) {
        conf = (conf & ~HALF_M_MASK) | (u << 9);
    }    
    
    /* read write 16 bits for the whole move count */
    inline unsigned get_wholemoves() const {
        return (conf & WHOLE_M_MASK) >> 16;
    }
    inline void set_wholemoves(unsigned u) {
        conf = (conf & ~WHOLE_M_MASK) | (u << 16);
    }
    
    
    /******************This is an XOR version of get/set epfile, half and whole. Comparable speed
    // read write 3 bits representing ep file 
    inline unsigned get_ep_file() {
        return (conf & EP_FILE_MASK) >> 6;
    }
    inline void set_ep_file(unsigned u) {
        unsigned c = conf ^ (u << 6);
        conf ^= (c & EP_FILE_MASK);
    }
    // for convenience, return the actual square
    inline Square get_ep_sq() {
        Square mksq(int, int);
        return mksq(get_white() ? 5 : 2, get_ep_file());
    }

    // read write 7 bits for the half move count 
    inline unsigned get_halfmoves() {
        return (conf & HALF_M_MASK) >> 9;
    }
    inline void set_halfmoves(unsigned u) {
        unsigned c = conf ^ (u << 9);
        conf ^= (c & HALF_M_MASK);
    }
    
    // read write 16 bits for the half move count 
    inline unsigned get_wholemoves() {
        return (conf & WHOLE_M_MASK) >> 16;
    }
    inline void set_wholemoves(unsigned u) {
        unsigned c = conf ^ (u << 16);
        conf ^= (c & WHOLE_M_MASK);
    } */

};


#endif