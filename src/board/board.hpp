#ifndef STASE_BOARD_HPP
#define STASE_BOARD_HPP

#include "../../include/stase/board.h"

#include <stdint.h>
#include <string>
#include <sstream>
using std::stringstream;
using std::string;

/*
 * This file defines a board representation.
 * 
 * It defines a basic interface for interacting with a chess board:
 *
 *   64 squares and a configuration word (move, castling, ep, half moves, whole moves)
 *
 *      -Board::get():       return the current piece on a square
 *      -Board::set():       set the piece of a square
 *
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
 */

/**
 * Namespace containing the bitmasks used to read and write the config word.
 */
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
    /** Returns the square on which an en-passant capture could take place */
    inline Square get_ep_sq() const {
        return get_white()
            ? mksq(get_ep_file(), 5)
            : mksq(get_ep_file(), 2);
    }
    /** Returns the square on which the pawn stands which can now be captured en-passant */
    inline Square get_ep_pawn_square() const {
        // on white's turn, we're looking for the black pawn's square
        // on black's turn, we're looking for the white pawn's square
        return get_white()
            ? mksq(get_ep_file(), 4)
            : mksq(get_ep_file(), 3);
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

    /**
     * Returns true if the current position is equivalent to the other position for the purpose of
     * threefold repetition.
     */
    bool equivalent(const Board & o) {
        if (non_move_conf() != o.non_move_conf()) {
            return false;
        }
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                if (squares[x][y] != o.squares[x][y]) {
                    return false;
                }
            }
        }
        return true;
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

    /**
     * Returns a version of the conf word which does not include the whole or half move count (ie, the bits
     * are set to zero).
     */
    uint32_t non_move_conf() const {
        return (conf & ~__bit_masks::WHOLE_M_MASK) & ~__bit_masks::HALF_M_MASK;
    }
};

#endif //STASE_BOARD_HPP
