#include "board.h"

#include <stdint.h>
#include <string>
#include <vector>
using std::vector;
using std::string;

#include <iostream>
using std::cout;

/* defines a move data type and defines functions to make moves, and return legal moves etc */

/* 
from (8 bits)
to (8 bits)
flags (16 bits):
    - bit 0-1:      promotion piece
    - bit 2:        is_prom
    - bit 3:        is_castle
    - bit 4:        is_enpassant
    - bit 5:        is_capture
    - bit 6:        castle side
    - bit 7-10:      captured piece
 */

// masks for reading flags etc
const unsigned PROM_FLAG = 4;
const unsigned CAS_FLAG = 8;
const unsigned EP_FLAG = 16;
const unsigned CAP_FLAG = 32;
const unsigned CAS_SHORT_FLAG = 64;

const unsigned PROM_PIECE_OFFSET = 0;
const unsigned PROM_PIECE_MASK = 3 << PROM_PIECE_OFFSET;
const unsigned CAP_PIECE_OFFSET = 6;
const unsigned CAP_PIECE_MASK = 15 << CAP_PIECE_OFFSET;

/* get/set promotion, castle flags etc */
bool Move::is_prom() const { return flags & PROM_FLAG; }
void Move::set_prom() { flags |= PROM_FLAG; }
bool Move::is_cas() const { return flags & CAS_FLAG; }
void Move::set_cas() { flags |= CAS_FLAG; }
bool Move::is_ep() const { return flags & EP_FLAG; }
void Move::set_ep() { flags |= EP_FLAG; }
bool Move::is_cap() const { return flags & CAP_FLAG; }
void Move::set_cap() { flags |= CAP_FLAG; }
bool Move::is_cas_short() const { return flags & CAS_SHORT_FLAG; }
void Move::set_cas_short() { flags |= CAS_SHORT_FLAG; }

// convenience: get the ep-file
int Move::get_ep_file() const { return get_x(to); }

/* get/set promotion piece */
Piece Move::get_prom_piece() const { 
    return QUEEN + ((flags & PROM_PIECE_MASK) >> PROM_PIECE_OFFSET);
}
void Move::set_prom_piece(unsigned u) {
    flags &= ~PROM_PIECE_MASK;
    flags |= (u << PROM_PIECE_OFFSET);
}

/* get/set captured piece */
Piece Move::get_cap_piece() const { 
    return (Piece) ((flags & CAP_PIECE_MASK) >> CAP_PIECE_OFFSET);
}
void Move::set_cap_piece(Piece p) {
    flags &= ~CAP_PIECE_MASK;
    flags |= (p << CAP_PIECE_OFFSET);
}

void make_move(Board & b, Move m) {
    // can't do castles or en-passant
    Piece p = b.get(m.from);
    b.set(m.to, p);
    b.set(m.from, EMPTY);

}

Bitmap line_search(const Board & b, const Square s,
                    void step(Square &), 
                    bool valid(const Square &)) {
    
    Bitmap bmap = (Bitmap) 0;
    
    Piece p = b.get(s);
    Square temp = s;
    bool cont = true;
    
    step(temp);
    
    while (valid(temp) && cont) {
        //cout << sqtos(temp);
        Piece otherp = b.get(temp);
        if (type(otherp) == EMPTY) {
            //cout << "e ";
            set_square(bmap, temp);
        } else {
            cont = false;
            if (colour(otherp) != colour(p)) {
                //cout << sqtos(temp) << "x ";
                set_square(bmap, temp);
            }
        }
        step(temp);
    }
    
    return bmap;
}

Bitmap ortho(const Board & b, const Square start_sq) {
    
    return line_search(b, start_sq, inc_x, val_x) 
            | line_search(b, start_sq, dec_x, val_x)
            | line_search(b, start_sq, inc_y, val_y)
            | line_search(b, start_sq, dec_y, val_y);
    
}

Bitmap diag(const Board & b, const Square start_sq) {
    
    return line_search(b, start_sq, diag_ur, val)
            | line_search(b, start_sq, diag_dl, val)
            | line_search(b, start_sq, diag_dr, val)
            | line_search(b, start_sq, diag_ul, val);
    
}

Bitmap knight_moves(const Board & b, const Square s) {

    Bitmap bmap;

    unsigned x = get_x(s), y = get_y(s);
    Ptype knightcol = colour(b.get(s));
    Square sq;
    
    if (val(sq = mksq(x + 1, y + 2)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x + 1, y - 2)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x + 2, y + 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x + 2, y - 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 1, y + 2)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 1, y - 2)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 2, y + 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 2, y - 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);

    return bmap;

}

Bitmap king_moves(const Board & b, const Square s) {
    
    Bitmap bmap;
    
    unsigned x = get_x(s), y = get_y(s);
    Ptype kingcol = colour(b.get(s));
    Square sq;
    
    if (val(sq = mksq(x + 1, y + 1)) && colour(b.get(sq)) != kingcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x + 1, y)) && colour(b.get(sq)) != kingcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x + 1, y - 1)) && colour(b.get(sq)) != kingcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x, y + 1)) && colour(b.get(sq)) != kingcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x, y - 1)) && colour(b.get(sq)) != kingcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 1, y + 1)) && colour(b.get(sq)) != kingcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 1, y)) && colour(b.get(sq)) != kingcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 1, y - 1)) && colour(b.get(sq)) != kingcol)
        set_square(bmap, sq);
    
    return bmap;
    
}

Bitmap pawn_moves(const Board & b, const Square s) {
    
    Bitmap bmap;
    
    unsigned x = get_x(s), y = get_y(s);
    Ptype pawncolour = colour(b.get(s));
    Square sq;
    
    unsigned FORWARD;
    unsigned START_RANK;
    Ptype capture_colour;
    
    if (pawncolour == WHITE) {
        FORWARD = 1;
        START_RANK = 1;
        capture_colour = BLACK;
    } else {
        FORWARD = -1;
        START_RANK = 6;
        capture_colour = WHITE;
    }
    
    // forward moves
    if (val(sq = mksq(x, y + FORWARD)) && b.get(sq) == EMPTY) {
        set_square(bmap, sq);
        if (y == START_RANK && b.get(sq = mksq(x, y + FORWARD + FORWARD)) == EMPTY) {
            set_square(bmap, sq);
        }
    }
    
    // regular captures
    if (val(sq = mksq(x - 1, y + FORWARD)) && colour(b.get(sq)) == capture_colour)
        set_square(bmap, sq);
    if (val(sq = mksq(x + 1, y + FORWARD)) && colour(b.get(sq)) == capture_colour)
        set_square(bmap, sq);
    
    // en-passant capture
    if (b.get_ep_exists() && y == START_RANK + 3*FORWARD) {
        unsigned epfile = b.get_ep_file();
        if (epfile == x + 1 || epfile == x - 1) {
            set_square(bmap, b.get_ep_sq());
        }
    }
    
    return bmap;
    
}

Bitmap piecemoves_ignore_check(const Board & b, const Square s) {

    Piece p = b.get(s);

    switch (type(p)) {
        
        case ROOK: 
            return ortho(b, s);
            
        case BISHOP: 
            return diag(b, s);
            
        case QUEEN: 
            return ortho(b, s) | diag(b, s);
            
        case KNIGHT:
            return knight_moves(b, s);
            
        case KING:
            return king_moves(b, s);
            
        case PAWN:
            return pawn_moves(b, s);
            
        default: 
            return (Bitmap) 0;
    }

}

void piecemoves_ignore_check(const Board & b, const Square s, vector<Move> & vec) {
    
    Bitmap bmap = piecemoves_ignore_check(b, s);
    Square newsq;
    
    for(int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            newsq = mksq(i, j);
            if (test_square(bmap, newsq)) {
                Move m;
                m.from = s;
                m.to = newsq;
                vec.push_back(m);
            }
        }
    }
    
}

Bitmap piecemoves(const Board & b, const Square) {
    return (Bitmap) 0;
}

void piecemoves(const Board & b, const Square, vector<Move> & vec) {

}

