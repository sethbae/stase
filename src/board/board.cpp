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

/**
 * Sneak is used to make a move on the board VERY temporarily - just to check something
 * (eg weak square? in check?) about the resulting position. It makes the move without
 * updating any conf info.
 * You can only SNEAK if you promise to immediately UNSNEAK!
 */
Piece Board::sneak(const Move m) const {

    Piece p = this->get(m.from);
    Piece captured = this->get(m.to);

    // set the from square to empty
    squares[m.from.x][m.from.y] = EMPTY;

    /*
     * Set the to square to the piece
     */
    squares[m.to.x][m.to.y] = p;

    return captured;

}

/**
 * Unsneak is used to undo a sneak operation. It undoes the given move, which should be
 * the same move passed to SNEAK. You should only UNSNEAK after a SNEAK, and you should
 * always UNSNEAK as soon as possible!
 *
 * The captured piece was returned to you from the SNEAK. Pass it back in to avoid
 * vanishing pieces!
 */
void Board::unsneak(const Move m, const Piece captured) const {

    Piece p = this->get(m.to);

    // set the to square to the captured piece
    squares[m.to.x][m.to.y] = captured;

    // set the from square to the piece
    squares[m.from.x][m.from.y] = p;
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
Byte Board::get_ep_file() const {
    return (conf & EP_FILE_MASK) >> 6;
}
void Board::set_ep_file(Byte u) {
    conf = (conf & ~EP_FILE_MASK) | (u << 6);
}

// for convenience, return the actual square
Square Board::get_ep_sq() const {
    return get_white()
            ? Square{ get_ep_file(), 5}
            : Square{get_ep_file(), 2};
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
            if (equal(m.to, Square{6, 0})) {
                set(mksq(7, 0), EMPTY);
                set(mksq(5, 0), W_ROOK);
            }
            
            // white long
            else if (equal(m.to, Square{2, 0})) {
                set(mksq(0, 0), EMPTY);
                set(mksq(3, 0), W_ROOK);
            }
            
            // black short
            else if (equal(m.to, Square{6, 7})) {
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
        Piece prom_piece = m.get_prom_piece(colour(get(m.to)));
        set(m.to, prom_piece);
    }
    
    return;
}

/**
 * Updates the config information for the given board, assuming the move m has just been moved.
 * This updates castling rights, en-passant, turn etc.
 */
void update_config_after_move(Board & b, const Move m) {
    
    // if white and there are castling rights to lose
    if (b.get_white() && (b.get_cas_ws() || b.get_cas_wl())) {
        
        // castling rights (from king move)
        if (m.is_cas() || b.get(m.from) == W_KING) {
            b.set_cas_ws(false);
            b.set_cas_wl(false);
        }
        
        // castling rights from rook move
        if (b.get(m.from) == W_ROOK) {
            if (equal(m.from, Square{7, 0})) {
                b.set_cas_ws(false);
            } else if (equal(m.from, Square{0, 0})) {
                b.set_cas_wl(false);
            }
        }
    
    }
    
    // if black and there are castling rights to lose
    if (!b.get_white() && (b.get_cas_bs() || b.get_cas_bl())) {
        
        // castling rights (from king move)
        if (m.is_cas() || b.get(m.from) == B_KING) {
            b.set_cas_bs(false);
            b.set_cas_bl(false);
        }
        
        // castling rights from rook move
        if (b.get(m.from) == B_ROOK) {
            if (equal(m.from, Square{7, 7})) {
                b.set_cas_bs(false);
            } else if (equal(m.from, Square{0, 7})) {
                b.set_cas_bl(false);
            }
        }
    
    }
    
    // en-passant
    if (type(b.get(m.from)) == PAWN) {
        if ((b.get_white() && get_y(m.to) - get_y(m.from) == 2)
                || (!b.get_white() && get_y(m.to) - get_y(m.from) == -2)) {
            b.set_ep_exists(true);
            b.set_ep_file(get_x(m.from));
        } else {
            b.set_ep_exists(false);
        }
    } else {
        b.set_ep_exists(false);
    }

    // half moves
    if (m.is_cap() || type(b.get(m.from)) == PAWN) {
        b.set_halfmoves(0);
    } else {
        b.inc_halfmoves();
    }
    
    // whole moves
    if (!b.get_white())
        b.inc_wholemoves();
    
    // turn
    b.flip_white();

}

/**
 * Creates and returns a new Board, with the given move having been played.
 * Updates turn, castling rights etc. This trusts that the move has all of
 * its flags set correctly, if that is not necessarily true then use the
 * successor_hard method.
 */
Board Board::successor(const Move m) const {

    // create new board and mutate it by the move
    Board b = *this;
    b.mutate(m);
    update_config_after_move(b, m);

    return b;
}

/**
 * Acts in the same way as successor, but does not trust the flags provided by the move given.
 */
Board Board::successor_hard(const Move m) const {

    // create new board and mutate it by the move
    Board b = *this;
    b.mutate_hard(m);
    update_config_after_move(b, m);

    return b;
}
