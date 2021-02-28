#include "board.h"

#include <stdint.h>
#include <string>
#include <vector>
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

void line_search(const Board & b, const Square s, Bitmap & bmap,
                    void step(Square &), 
                    bool valid(const Square &)) {
    
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
}

void ortho(const Board & b, const Square start_sq, Bitmap & bmap) {
    
    line_search(b, start_sq, bmap, inc_x, val_x);
    line_search(b, start_sq, bmap, dec_x, val_x);
    line_search(b, start_sq, bmap, inc_y, val_y);
    line_search(b, start_sq, bmap, dec_y, val_y);
    
}

void diag(const Board & b, const Square start_sq, Bitmap & bmap) {
    
    line_search(b, start_sq, bmap, diag_ur, val);
    line_search(b, start_sq, bmap, diag_dl, val);
    line_search(b, start_sq, bmap, diag_dr, val);
    line_search(b, start_sq, bmap, diag_ul, val);
    
}

void knight_moves(const Board & b, const Square s, Bitmap & bmap) {

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

}

void king_moves(const Board & b, const Square s, Bitmap & bmap) {
    
    unsigned x = get_x(s), y = get_y(s);
    Ptype knightcol = colour(b.get(s));
    Square sq;
    
    if (val(sq = mksq(x + 1, y + 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x + 1, y)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x + 1, y - 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x, y + 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x, y - 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 1, y + 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 1, y)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    if (val(sq = mksq(x - 1, y - 1)) && colour(b.get(sq)) != knightcol)
        set_square(bmap, sq);
    
}

void pawn_moves(const Board & b, const Square s, Bitmap & bmap) {
    
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
    
}

void piecemoves(const Board & b, const Square s, Bitmap & bmap) {

    Piece p = b.get(s);

    switch (type(p)) {
        
        case ROOK: 
            ortho(b, s, bmap);
            break;
            
        case BISHOP: 
            diag(b, s, bmap);
            break;
            
        case QUEEN: 
            ortho(b, s, bmap);
            diag(b, s, bmap);
            break;
            
        case KNIGHT:
            knight_moves(b, s, bmap);
            break;
            
        case KING:
            king_moves(b, s, bmap);
            break;
            
        case PAWN:
            pawn_moves(b, s, bmap);
            break;
            
        default: 
            return;
    }

}


// Produces undefined behaviour if the move is not a sliding move
bool is_unobstructed(Move m, Bitmap vacancy) {
    int xs = get_x(m.from), ys = get_y(m.from), xe = get_x(m.to), ye = get_y(m.to);

    uint64_t v_range, h_range;          // To account for pieces going backwards...

    if (xs > xe) {
        h_range = columns_map(xe, xs);
    } else {
        h_range = columns_map(xs, xe);
    }

    if (ys > ye) {
        v_range = rows_map(ye, ys);
    } else {
        v_range = rows_map(ys, ye);
    }

    // The box contains only the squares within the smallest square containing both the start and end square
    uint64_t box = v_range & h_range;

    // pos is the representation of both the start and end squares only
    uint64_t pos = square_map(m.from) | square_map(m.to);

    // The only straight line connecting both squares (needs optimisation imo)
    uint64_t line;
    int diffx = xe - xs;
    int diffy = ye - ys;
    if (diffx == 0) {               // if it's vertical
        line = column_map(xe);
    } else if (diffy == 0) {        // if it's horizontal
        line = row_map(ye);
    } else if (diffy == diffx) {    // if it's a positive diagonal
        line = posdiag_map(m.from);
    } else {                        // if it's a negative diagonal
        line = negdiag_map(m.to);
    }
    
    uint64_t path = box & line & ~pos;
    // pr_mask(box);

    // The path between a sqaure and another would be unobstructed only if there is no occupied square in between them
    return (path & vacancy) == path;

}

Bitmap attack_map(const Board & b) {
    // Bitmap movable_pieces = custom_map(b, [] (Square s, Piece p, Ptype c) { return colour(p) == c; });
    // Bitmap enemy_pieces = custom_map(b, [] (Square s, Piece p, Ptype c) {return colour(p) != c;});
    Bitmap attack_map = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Square pos = mksq(i, j);
            piecemoves(b, pos, attack_map);

        }
    }

    return attack_map;
}

bool is_legal_example(Move m, const Board & b, Bitmap movable_pieces, Bitmap enemy_pieces, Bitmap vacancy) {
    Bitmap move_pattern = pattern_map(m.from, b.get(m.from));


    // Checks for basic piece movement
    if ((move_pattern & ~square_map(m.from) & square_map(m.to)) == 0) {
        return false;
    }

    // Checks the captures are of the right colour
    if (b.get(m.to) != EMPTY && colour(b.get(m.to)) == colour(b.get(m.from))) {
        return false;
    }

    // Checks for obstructions
    Ptype p = type(b.get(m.from));
    if ((p == QUEEN || p == ROOK || p == BISHOP) && !is_unobstructed(m, vacancy)) {
        return false;
    }

    // Checks if the location the king is moving to is attacked by enemy pieces
    /* if (p == KING && ((attack_map(b, vacancy, enemy_pieces, movable_pieces) & square_map(m.to)) != 0)) {
        return false;
    } */

    // Other legality tests...

    return true;
}

