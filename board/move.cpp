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
Byte Move::get_prom_shift() const { 
    return (flags & PROM_PIECE_MASK) >> PROM_PIECE_OFFSET;
}

// return the actual piece, given the colour it should be
Piece Move::get_prom_piece(Ptype col) const {
    Piece p = (col == WHITE) ? W_QUEEN : B_QUEEN;
    return p + get_prom_shift();
}

void Move::set_prom_piece(Ptype p) {
    unsigned u = p - (unsigned) QUEEN;
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

Move empty_move() {
    Move m = {0, 0, 0};
    return m;
}

void make_move(Board & b, Move m) {
    
    Ptype col = colour(b.get(m.from));
    
    // castling
    if (m.is_cas()) {
   
        if (col == WHITE) {
            if (m.is_cas_short()) {
                b.set(mksq(7, 0), EMPTY);   // white short
                b.set(mksq(5, 0), W_ROOK);
            } else {
                b.set(mksq(0, 0), EMPTY);   // white long
                b.set(mksq(3, 0), W_ROOK);
            }
        }
        
        else {
            if (m.is_cas_short()) {
                b.set(mksq(7, 7), EMPTY);   // white short
                b.set(mksq(5, 7), B_ROOK);
            } else {
                b.set(mksq(0, 7), EMPTY);   // white long
                b.set(mksq(3, 7), B_ROOK);  
            }
        }
            
    }
    
    // en-passant
    if (m.is_ep()) {
        if (col == WHITE) {
            b.set(mksq(m.get_ep_file(), 4), EMPTY);
        } else {
            b.set(mksq(m.get_ep_file(), 3), EMPTY);
        }
    }
       
    // move piece
    Piece p = b.get(m.from);
    b.set(m.to, p);
    b.set(m.from, EMPTY);

    // promotion
    if (m.is_prom()) {
        Piece p = m.get_prom_piece(col);
        b.set(m.to, p);
    }
    
    return;
}

// version of make move which doesn't assume the move is correctly configured
void make_move_hard(Board & b, Move m) {
    
    // castling
    if (type(b.get(m.from)) == KING) {
        int delta = get_x(m.from) - get_x(m.to);
        if (delta == 2 || delta == -2) {
            
            // white short
            if (m.to == mksq(6, 0)) {
                b.set(mksq(7, 0), EMPTY);
                b.set(mksq(5, 0), W_ROOK);
            }
            
            // white long
            else if (m.to == mksq(2, 0)) {
                b.set(mksq(0, 0), EMPTY);
                b.set(mksq(3, 0), W_ROOK);
            }
            
            // black short
            else if (m.to == mksq(6, 7)) {
                b.set(mksq(7, 7), EMPTY);
                b.set(mksq(5, 7), B_ROOK);
            }
            
            // black long
            else {
                b.set(mksq(0, 7), EMPTY);
                b.set(mksq(3, 7), B_ROOK);
            }
        }
    }
    
    // en-passant
    if (type(b.get(m.from)) == PAWN && get_x(m.from) != get_x(m.to)) {
        if (b.get(m.to) == EMPTY) {
            
            // white
            if (get_y(m.to) == 5) {
                b.set(mksq(get_x(m.to), 4), EMPTY);
            }
            
            // black   
            else {
                b.set(mksq(get_x(m.to), 3), EMPTY);
            }            
        }
    }
       
    // move piece
    Piece p = b.get(m.from);
    b.set(m.to, p);
    b.set(m.from, EMPTY);

    // promotion
    if (type(b.get(m.to)) == PAWN && (get_y(m.to) == 7 || get_y(m.to) == 0)) {
        Piece p = m.get_prom_piece(colour(b.get(m.to)));
        b.set(m.to, p);
    }
    
    return;
}

Bitmap line_search(const Board & b, const Square s,
                    void step(Square &), 
                    bool (* valid)(const Square &)) {
    
    Bitmap bmap = (Bitmap) 0;
    
    Piece p = b.get(s);
    Square temp = s;
    bool cont = true;
    
    step(temp);
    
    while ((*valid)(temp) && cont) {
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

    Bitmap bmap = 0;

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
    
    Bitmap bmap = 0;
    
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
    
    Bitmap bmap = 0;
    
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
            
        case KING: {
            return king_moves(b, s);
        }
            
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

bool line_search_check(const Board & b, Square sq, const Piece p1, const Piece p2,
                        void step(Square &),
                        bool valid(const Square &)) {
                                
    step(sq);
    
    while (valid(sq)) {

        Piece otherp = b.get(sq);
        
        if (type(otherp) != EMPTY) {
            
            if (otherp == p1 || otherp == p2) {
                return true;
            } else {
                return false;
            }
            
        }
        step(sq);
    }
    
    return false;                        
                        
}

/* returns true iff the player to move is in check */
bool in_check_hard(const Board & b) {

    bool white = b.get_white();
    Piece king = (white ? W_KING : B_KING);
    Square ksq;
    unsigned king_x, king_y;
    
    // find king square
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (b.get(mksq(i, j)) == king) {
                ksq = mksq(i, j);
                king_x = i;
                king_y = j;
            }
        }
    }
    
    // check pawns
    if (white) {
        
        if (king_x > 0 && king_y < 7 && b.get(mksq(king_x - 1, king_y + 1)) == B_PAWN) {
            return true;
        }
        if (king_x < 7 && king_y < 7 && b.get(mksq(king_x + 1, king_y + 1)) == B_PAWN) {
           return true;
        }
        
    } else {
        
        if (king_x > 0 && king_y > 0 && b.get(mksq(king_x - 1, king_y - 1)) == W_PAWN) {
            return true;
        }
        if (king_x < 7 && king_y > 0 && b.get(mksq(king_x + 1, king_y - 1)) == W_PAWN) {
           return true;
        }
    
    }
    
    // check knight
    Piece enemy_knight = white ? B_KNIGHT : W_KNIGHT;
    Square sq;
    if (val(sq = mksq(king_x + 1, king_y + 2)) && b.get(sq) == enemy_knight)
        return true;
    if (val(sq = mksq(king_x + 1, king_y - 2)) && b.get(sq) == enemy_knight)
        return true;
    if (val(sq = mksq(king_x + 2, king_y + 1)) && b.get(sq) == enemy_knight)
        return true;
    if (val(sq = mksq(king_x + 2, king_y - 1)) && b.get(sq) == enemy_knight)
        return true;
    if (val(sq = mksq(king_x - 1, king_y + 2)) && b.get(sq) == enemy_knight)
        return true;
    if (val(sq = mksq(king_x - 1, king_y - 2)) && b.get(sq) == enemy_knight)
        return true;
    if (val(sq = mksq(king_x - 2, king_y + 1)) && b.get(sq) == enemy_knight)
        return true;
    if (val(sq = mksq(king_x - 2, king_y - 1)) && b.get(sq) == enemy_knight)
        return true;
        
    // search out from king
    
    Piece queen, bishop, rook;
    if (white) {
        queen = B_QUEEN;
        rook = B_ROOK;
        bishop = B_BISHOP;
    } else {
        queen = W_QUEEN;
        rook = W_ROOK;
        bishop = W_BISHOP;
    }
    
    // diagonal: bishop or queen
    if (line_search_check(b, ksq, bishop, queen, diag_ur, val)
         || line_search_check(b, ksq, bishop, queen, diag_dr, val)
         || line_search_check(b, ksq, bishop, queen, diag_ul, val)
         || line_search_check(b, ksq, bishop, queen, diag_dl, val)) {
        return true;    
    }
            
    // orthogonal: rook or queen
    if (line_search_check(b, ksq, queen, rook, inc_x, val_x)
         || line_search_check(b, ksq, queen, rook, dec_x, val_x)
         || line_search_check(b, ksq, queen, rook, inc_y, val_y)
         || line_search_check(b, ksq, queen, rook, dec_y, val_y)) {
        return true;
    }
    
    return false;
}



bool in_check_attack_map(const Board & b, Ptype c) {

    Bitmap king_pos = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square pos = mksq(i, j);
            Piece p = b.get(pos);
            if (type(p) == KING && colour(p) == c) {
                set_square(king_pos, pos);
                goto end_loop;
            }
        }
    }
    end_loop:

    Ptype enemy = (c == WHITE) ? BLACK : WHITE;
    Bitmap attack = attack_map(b, enemy);

    // Both the king position and the attack map can be maintained and updated every move, which can make this function O(1)
    return king_pos & attack != 0;
}
