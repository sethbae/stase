#ifndef STASE_BOARD_HPP
#define STASE_BOARD_HPP

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

namespace __bit_masks {
    const unsigned WHITE_MASK = 1 << 0;
    const unsigned CAS_WS_MASK = 1 << 1;
    const unsigned CAS_WL_MASK = 1 << 2;
    const unsigned CAS_BS_MASK = 1 << 3;
    const unsigned CAS_BL_MASK = 1 << 4;
    const unsigned EP_EX_MASK = 1 << 5;
    const unsigned EP_FILE_MASK = 7 << 6;
    const unsigned HALF_M_MASK = 127 << 9;
    const unsigned WHOLE_M_MASK = (unsigned) (~0) << 16;
}

struct Board {

    mutable Piece squares[8][8];
    Int conf;

    inline Piece get(const int x, const int y) const {
        return squares[x][y];
    };
    inline void set(const int x, const int y, const Piece p) {
        squares[x][y] = p;
    }
    inline Piece get(const Square & s) const {
        return squares[s.x][s.y];
    };
    inline void set(const Square & s, const Piece p) {
        squares[s.x][s.y] = p;
    }

    /**
     * Sneak is used to make a move on the board VERY temporarily - just to check something
     * (eg weak square? in check?) about the resulting position. It makes the move without
     * updating any conf info.
     * You can only SNEAK if you promise to immediately UNSNEAK!
     */
    inline Piece sneak(const Move m) const {
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
    inline void unsneak(const Move m, const Piece captured) const {

        Piece p = this->get(m.to);

        // set the to square to the captured piece
        squares[m.to.x][m.to.y] = captured;

        // set the from square to the piece
        squares[m.from.x][m.from.y] = p;
    }

    inline void set_conf_word(Int c) { conf = c; }
    inline Int get_conf_word() const { return conf; }

    /**
     * Returns true iff it is white's turn to play
     */
    inline bool get_white() const { return conf & __bit_masks::WHITE_MASK; }
    inline void set_white(bool b) {
        if (b)
            conf |= __bit_masks::WHITE_MASK;
        else
            conf &= ~__bit_masks::WHITE_MASK;
    }
    inline void flip_white() { conf ^= __bit_masks::WHITE_MASK; }
    inline Colour colour_to_move() const {
        return (conf & __bit_masks::WHITE_MASK) ? WHITE : BLACK;
    }

    /**
     * Get/set castling rights individually
     * */
    inline bool get_cas_ws() const { return conf & __bit_masks::CAS_WS_MASK; }
    inline void set_cas_ws(bool b) {
        if (b)
            conf |= __bit_masks::CAS_WS_MASK;
        else
            conf &= ~__bit_masks::CAS_WS_MASK;
    }
    inline bool get_cas_wl() const { return conf & __bit_masks::CAS_WL_MASK; }
    inline void set_cas_wl(bool b) {
        if (b)
            conf |= __bit_masks::CAS_WL_MASK;
        else
            conf &= ~__bit_masks::CAS_WL_MASK;
    }
    inline bool get_cas_bs() const { return conf & __bit_masks::CAS_BS_MASK; }
    inline void set_cas_bs(bool b) {
        if (b)
            conf |= __bit_masks::CAS_BS_MASK;
        else
            conf &= ~__bit_masks::CAS_BS_MASK;
    }
    inline bool get_cas_bl() const { return conf & __bit_masks::CAS_BL_MASK; }
    inline void set_cas_bl(bool b) {
        if (b)
            conf |= __bit_masks::CAS_BL_MASK;
        else
            conf &= ~__bit_masks::CAS_BL_MASK;
    }

    /**
     * Get/set en-passant boolean
     */
    inline bool get_ep_exists() const { return conf & __bit_masks::EP_EX_MASK; }
    inline void set_ep_exists(bool b) {
        if (b)
            conf |= __bit_masks::EP_EX_MASK;
        else
            conf &= ~__bit_masks::EP_EX_MASK;
    }

    /**
     * Read/write 3 bits representing ep file
     */
    inline Byte get_ep_file() const {
        return (conf & __bit_masks::EP_FILE_MASK) >> 6;
    }
    inline void set_ep_file(Byte u) {
        conf = (conf & ~__bit_masks::EP_FILE_MASK) | (u << 6);
    }
    inline Square get_ep_sq() const {
        return get_white()
            ? mksq(get_ep_file(), 5)
            : mksq(get_ep_file(), 2);
    }
    inline Square get_ep_pawn_square() const {
        return get_white()
            ? mksq(get_ep_file(), 3)
            : mksq(get_ep_file(), 4);
    }

    /**
     * Read/write 7 bits for the half move count
     */
    inline unsigned get_halfmoves() const {
        return (conf & __bit_masks::HALF_M_MASK) >> 9;
    }
    inline void set_halfmoves(unsigned u) {
        conf = (conf & ~__bit_masks::HALF_M_MASK) | (u << 9);
    }
    inline void inc_halfmoves() { conf += 1 << 9; }

    /**
     * Read/write 16 bits for the whole move count
     */
    inline unsigned get_wholemoves() const {
        return (conf & __bit_masks::WHOLE_M_MASK) >> 16;
    }
    inline void set_wholemoves(unsigned u) {
        conf = (conf & ~__bit_masks::WHOLE_M_MASK) | (u << 16);
    }
    inline void inc_wholemoves() { conf += (1 << 16); }

    /* change the position of the pieces, without affecting config */
    void mutate(const Move m) {

        Colour col = colour_to_move();

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

    void mutate_hard(const Move m) {

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
     * Creates and returns a new Board, with the given move having been played.
     * Updates turn, castling rights etc. This trusts that the move has all of
     * its flags set correctly, if that is not necessarily true then use the
     * successor_hard method.
     */
    Board successor(const Move m) const {

        // create new board and mutate it by the move
        Board b = *this;
        b.mutate(m);
        update_config_after_move(b, m);

        return b;
    }

    /**
     * Acts in the same way as successor, but does not trust the flags provided by the move given.
     */
    Board successor_hard(const Move m) const {

        // create new board and mutate it by the move
        Board b = *this;
        b.mutate_hard(m);
        update_config_after_move(b, m);

        return b;
    }

private:

    /**
   * Updates the config information for the given board, assuming the move m has just been moved.
   * This updates castling rights, en-passant, turn etc.
   * This must be called after the board has been updated to the new position. If it is called
   * beforehand, it will not work correctly.
   */
    void update_config_after_move(Board & b, const Move m) const {

        // if white and there are castling rights to lose
        if (b.get_white() && (b.get_cas_ws() || b.get_cas_wl())) {

            // castling rights (from king move)
            if (b.get(m.to) == W_KING) {
                b.set_cas_ws(false);
                b.set_cas_wl(false);
            }

            // castling rights from rook move
            if (b.get(m.to) == W_ROOK) {
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
            if (b.get(m.to) == B_KING) {
                b.set_cas_bs(false);
                b.set_cas_bl(false);
            }

            // castling rights from rook move
            if (b.get(m.to) == B_ROOK) {
                if (equal(m.from, Square{7, 7})) {
                    b.set_cas_bs(false);
                } else if (equal(m.from, Square{0, 7})) {
                    b.set_cas_bl(false);
                }
            }

        }

        // en-passant
        if (type(b.get(m.to)) == PAWN) {
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
        if (m.is_cap() || type(b.get(m.to)) == PAWN) {
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

};

#endif //STASE_BOARD_HPP
