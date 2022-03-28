#include <string>
#include <vector>
using std::vector;
using std::string;
#include <iostream>
using std::cout;

#include "board.h"
#include "board.hpp"

/*
 * Defines a move data type and defines functions to make moves
 * and return legal moves etc
 */

/*
 * Layout is this:
 * from (8 bits)
 * to (8 bits)
 * flags (16 bits):
 *  - bit 0-1:      promotion piece
 *  - bit 2:        is_prom
 *  - bit 3:        is_castle
 *  - bit 4:        is_enpassant
 *  - bit 5:        is_capture
 *  - bit 6:        castle side
 *  - bit 7-10:     captured piece
 *  - bit 11:       unused
 *  - bit 12-15:    score
 */

// masks for reading flags etc
const int PROM_FLAG = 4;
const int CAS_FLAG = 8;
const int EP_FLAG = 16;
const int CAP_FLAG = 32;
const int CAS_SHORT_FLAG = 64;

const int PROM_PIECE_OFFSET = 0;
const int PROM_PIECE_MASK = 3 << PROM_PIECE_OFFSET;
const int CAP_PIECE_OFFSET = 6;
const int CAP_PIECE_MASK = 15 << CAP_PIECE_OFFSET;

const int SCORE_MASK = 0xF000;

/**
 * Values for the move diffs (cannot be defined in header).
 */
const SignedByte XD[] = {-1, -1, 1, 1, -1, 1, 0, 0};
const SignedByte YD[] = {-1, 1, -1, 1, 0, 0, 1, -1};

const SignedByte XKN[] = {1, 1, 2, 2, -1, -1, -2, -2};
const SignedByte YKN[] = {2, -2, 1, -1, 2, -2, 1, -1};

const Delta D[8] = {
        // the first four deltas originate on the left hand side, and go right
        delta(1, 0),
        delta(0, 1),
        delta(1, 1),
        delta(1, -1),
        // the final four deltas run counter to the originals
        delta(-1, 0),
        delta(0, -1),
        delta(-1, -1),
        delta(-1, 1)
};

const Delta D_ORTH[4] = {
        delta(1, 0),
        delta(-1, 0),
        delta(0, 1),
        delta(0, -1)
};

const Delta D_DIAG[4] = {
        delta(1, 1),
        delta(-1, 1),
        delta(1, -1),
        delta(-1, -1)
};

/* get/set promotion, castle flags etc */
bool Move::is_prom() const { return flags & PROM_FLAG; }
void Move::set_prom() { flags |= PROM_FLAG; }
void Move::unset_prom() { flags &= ~PROM_FLAG; }

bool Move::is_cas() const { return flags & CAS_FLAG; }
void Move::set_cas() { flags |= CAS_FLAG; }
void Move::unset_cas() { flags &= ~CAS_FLAG; }

bool Move::is_ep() const { return flags & EP_FLAG; }
void Move::set_ep() { flags |= EP_FLAG; }
void Move::unset_ep() { flags &= ~EP_FLAG; }

bool Move::is_cap() const { return flags & CAP_FLAG; }
void Move::set_cap() { flags |= CAP_FLAG; }
void Move::unset_cap() { flags &= ~CAP_FLAG; }

bool Move::is_cas_short() const { return flags & CAS_SHORT_FLAG; }
void Move::set_cas_short() { flags |= CAS_SHORT_FLAG; }
void Move::unset_cas_short() { flags &= ~CAS_SHORT_FLAG; }

// convenience: get the ep-file
int Move::get_ep_file() const { return get_x(to); }

/* get/set promotion piece */
Byte Move::get_prom_shift() const { 
    return (flags & PROM_PIECE_MASK) >> PROM_PIECE_OFFSET;
}

// return the actual piece, given the colour it should be
Piece Move::get_prom_piece(Colour col) const {
    Piece p = (col == WHITE) ? W_QUEEN : B_QUEEN;
    return (Piece) (p + get_prom_shift());
}

void Move::set_prom_piece(Ptype p) {
    int u = p - (int) QUEEN;
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

int Move::get_score() const {
    return (flags & SCORE_MASK) >> 12;
}

void Move::set_score(int score) {
    flags = (flags & ~SCORE_MASK) | (score << 12);
}

void Move::inc_score(int inc) {
    set_score(get_score() + inc);
}

/*************************************************************************
 *  VECTOR piece move functions
 ************************************************************************/

void line_search(const Board & b, const Square s,
                    Delta d,
                    vector<Move> & moves) {
        
    Piece p = b.get(s);
    Square temp = s;
    bool cont = true;
    Move m = empty_move();
    m.from = s;
    
    temp.x += d.dx;
    temp.y += d.dy;

    while (val(temp) && cont) {
        //cout << sqtos(temp);
        Piece otherp = b.get(temp);
        if (otherp == EMPTY) {
            //cout << "e ";
            m.to = temp;
            moves.push_back(m);
        } else {
            cont = false;
            if (colour(otherp) != colour(p)) {
                //cout << sqtos(temp) << "x ";
                m.to = temp;
                m.set_cap();
                m.set_cap_piece(otherp);
                moves.push_back(m);
            }
        }
        temp.x += d.dx;
        temp.y += d.dy;
    }

}

void ortho(const Board & b, const Square start_sq, vector<Move> & moves) {

    line_search(b, start_sq, Delta{1, 0}, moves);
    line_search(b, start_sq, Delta{-1, 0},moves);
    line_search(b, start_sq, Delta{0, 1}, moves);
    line_search(b, start_sq, Delta{0, -1}, moves);

}

void diag(const Board & b, const Square start_sq, vector<Move> & moves) {

    line_search(b, start_sq, Delta{1, 1}, moves);
    line_search(b, start_sq, Delta{-1, -1}, moves);
    line_search(b, start_sq, Delta{1, -1}, moves);
    line_search(b, start_sq, Delta{-1, 1}, moves);

}

void knight_moves(const Board & b, const Square s, vector<Move> & moves) {

    int x = get_x(s), y = get_y(s);
    Colour knightcol = colour(b.get(s));
    Square temp;

     for (int i = 0; i < 8; ++i) {
         if (val(temp = mksq(x + XKN[i], y + YKN[i])) && colour(b.get(temp)) != knightcol) {
             Move m{s, temp, 0};
             if (b.get(temp) != EMPTY) {
                 m.set_cap();
                 m.set_cap_piece(b.get(temp));
             }
             moves.push_back(m);
         }
     }

}

// checks the kings castling squares are empty and check-free: nothing more
bool castle_checks(Board b, const Colour col, const bool kingside) {
    
    Square s1, s2, s3, s4; // intermediate squares
    Square k_sq = (col == WHITE) ? stosq("e1") : stosq("e8");

    if (col == WHITE && kingside) {
        
        s1 = stosq("f1");
        s2 = stosq("g1");
        s3 = stosq("h1");
        
        if (b.get(s1) == EMPTY && b.get(s2) == EMPTY && b.get(s3) == W_ROOK) {
            
            b.set(k_sq, EMPTY);
            b.set(s1, W_KING);
            
            if (!in_check_hard(b, WHITE)) {
                b.set(s1, EMPTY);
                b.set(s2, W_KING);
                
                return !in_check_hard(b, WHITE);
            }
            
        }           
        
    } else if (col == WHITE && !kingside) {

        s1 = stosq("d1");
        s2 = stosq("c1");
        s3 = stosq("b1");
        s4 = stosq("a1");

        if (b.get(s1) == EMPTY && b.get(s2) == EMPTY && b.get(s3) == EMPTY && b.get(s4) == W_ROOK) {
            
            b.set(k_sq, EMPTY);
            b.set(s1, W_KING);
            
            if (!in_check_hard(b, WHITE)) {
                b.set(s1, EMPTY);
                b.set(s2, W_KING);
                
                return !in_check_hard(b, WHITE);
            }
            
        }   
                
    } else if (col == BLACK && kingside) {

        s1 = stosq("f8");
        s2 = stosq("g8");
        s3 = stosq("h8");

        if (b.get(s1) == EMPTY && b.get(s2) == EMPTY && b.get(s3) == B_ROOK) {
            
            b.set(k_sq, EMPTY);
            b.set(s1, B_KING);
            
            if (!in_check_hard(b, BLACK)) {
                b.set(s1, EMPTY);
                b.set(s2, B_KING);
                
                return !in_check_hard(b, BLACK);
            }
            
        }
                  
    } else { // BLACK and !kingside

        s1 = stosq("d8");
        s2 = stosq("c8");
        s3 = stosq("b8");
        s4 = stosq("a8");

        if (b.get(s1) == EMPTY && b.get(s2) == EMPTY && b.get(s3) == EMPTY && b.get(s4) == B_ROOK) {

            b.set(k_sq, EMPTY);
            b.set(s1, B_KING);

            if (!in_check_hard(b, BLACK)) {
                b.set(s1, EMPTY);
                b.set(s2, B_KING);

                return !in_check_hard(b, BLACK);
            }
            
        }
    }

    return false;
}

void king_moves(const Board & b, const Square s, vector<Move> & moves) {

    Colour kingcol = colour(b.get(s));
    Square temp;

    // check castle for white
    if (kingcol == WHITE && !in_check_hard(b, WHITE)) {
        
        if (b.get_cas_ws() && castle_checks(b, WHITE, true)) {
            Move m{s, stosq("g1"), 0};
            m.set_cas();
            m.set_cas_short();
            moves.push_back(m);
        }
        
        if (b.get_cas_wl() && castle_checks(b, WHITE, false)) {
            Move m{s, stosq("c1"), 0};
            m.set_cas();
            m.unset_cas_short();
            moves.push_back(m);
        }
    
    // and for black
    } else if (kingcol == BLACK && !in_check_hard(b, BLACK)) {

        if (b.get_cas_bs() && castle_checks(b, BLACK, true)) {
            Move m{s, stosq("g8"), 0};
            m.set_cas();
            m.set_cas_short();
            moves.push_back(m);
        }

        if (b.get_cas_bl() && castle_checks(b, BLACK, false)) {
            Move m{s, stosq("c8"), 0};
            m.set_cas();
            m.unset_cas_short();
            moves.push_back(m);
        }

    }

    // check all adjacent squares
    int x = get_x(s), y = get_y(s);
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(x + XD[i], y + YD[i])) && colour(b.get(temp)) != kingcol) {
            Move m{s, temp, 0};
            if (b.get(temp) != EMPTY) {
                m.set_cap();
                m.set_cap_piece(b.get(temp));
            }
            moves.push_back(m);
        }
    }

}

void pawn_moves(const Board & b, const Square s, vector<Move> & moves) {
       
    int x = get_x(s), y = get_y(s);
    Colour pawn_colour = colour(b.get(s));
    Square sq;
    
    Move m = empty_move();
    m.from = s;
    
    int FORWARD;
    int START_RANK;
    Colour capture_colour;
    
    if (pawn_colour == WHITE) {
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
        
        m.to = sq;
        
        // final rank: promotion
        if (get_y(sq) == 0 || get_y(sq) == 7) {
            m.set_prom();
            m.set_prom_piece(QUEEN);
            Move m2 = m;
            m2.set_prom_piece(ROOK);
            moves.push_back(m2);
            m2.set_prom_piece(BISHOP);
            moves.push_back(m2);
            m2.set_prom_piece(KNIGHT);
            moves.push_back(m2);
        }
        
        moves.push_back(m);
        
        // starting rank: double move
        if (y == START_RANK && b.get(sq = mksq(x, y + FORWARD + FORWARD)) == EMPTY) {
            m.to = sq;
            moves.push_back(m);
        } 
        
    }
    
    // regular captures
    if (val(sq = mksq(x - 1, y + FORWARD)) && colour(b.get(sq)) == capture_colour) {
        m.to = sq;
        m.set_cap();
        m.set_cap_piece(b.get(sq));
        // final rank: promotion
        if (get_y(sq) == 0 || get_y(sq) == 7) {
            m.set_prom();
            m.set_prom_piece(QUEEN);
            Move m2 = m;
            m2.set_prom_piece(ROOK);
            moves.push_back(m2);
            m2.set_prom_piece(BISHOP);
            moves.push_back(m2);
            m2.set_prom_piece(KNIGHT);
            moves.push_back(m2);
        }
        moves.push_back(m);
    }
    if (val(sq = mksq(x + 1, y + FORWARD)) && colour(b.get(sq)) == capture_colour) {
        m.to = sq;
        m.set_cap();
        m.set_cap_piece(b.get(sq));
        // final rank: promotion
        if (get_y(sq) == 0 || get_y(sq) == 7) {
            m.set_prom();
            m.set_prom_piece(QUEEN);
            Move m2 = m;
            m2.set_prom_piece(ROOK);
            moves.push_back(m2);
            m2.set_prom_piece(BISHOP);
            moves.push_back(m2);
            m2.set_prom_piece(KNIGHT);
            moves.push_back(m2);
        }
        moves.push_back(m);   
    }
    
    // en-passant capture
    if (b.get_ep_exists() && y == START_RANK + 3*FORWARD) {
        int epfile = b.get_ep_file();
        if (epfile == x + 1 || epfile == x - 1) {
            m.to = mksq(epfile, y + FORWARD);
            m.set_cap();
            m.set_cap_piece((pawn_colour == WHITE) ? B_PAWN : W_PAWN);
            m.set_ep();
            moves.push_back(m);
        }
    }

}

void piecemoves_ignore_check(const Board & b, const Square s, vector<Move> & moves) {

    Piece p = b.get(s);

    switch (type(p)) {
        
        case ROOK: 
            ortho(b, s, moves);
            break;
            
        case BISHOP: 
            diag(b, s, moves);
            break;
            
        case QUEEN: 
            ortho(b, s, moves); 
            diag(b, s, moves);
            break;
            
        case KNIGHT:
            knight_moves(b, s, moves);
            break;
            
        case KING:
            king_moves(b, s, moves);
            break;
            
        case PAWN:
            pawn_moves(b, s, moves);
            break;
            
        default: 
            return;
    }

}

/*************************************************************************
 *  In check functions
 ************************************************************************/

bool line_search_check(const Board & b, Square sq, const Piece p1, const Piece p2, Delta d) {
                                
    sq.x += d.dx;
    sq.y += d.dy;

    while (val(sq)) {
        Piece otherp = b.get(sq);
        if (otherp != EMPTY) {
            return otherp == p1 || otherp == p2;
        }
        sq.x += d.dx;
        sq.y += d.dy;
    }
    return false;
}

/**
 * Returns true iff the player of the given colour is in check. If no king of the given
 * colour is found, then false is returned.
 */
bool in_check_hard(const Board & b, Colour col) {

    Square temp;
    bool white = (col == WHITE);
    Piece king = (white) ? W_KING : B_KING;
    Square ksq{0, 0};
    int king_x = 0, king_y = 0;
    bool king_found = false;

    // find king square
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (b.get(mksq(i, j)) == king) {
                king_found = true;
                ksq = mksq(i, j);
                king_x = i;
                king_y = j;
            }
        }
    }

    if (!king_found) { return false; }

    // check pawns
    if (white) {
        if (king_x > 0 && king_y < 7 && b.get(mksq(king_x - 1, king_y + 1)) == B_PAWN) { return true; }
        if (king_x < 7 && king_y < 7 && b.get(mksq(king_x + 1, king_y + 1)) == B_PAWN) { return true; }
    } else {
        if (king_x > 0 && king_y > 0 && b.get(mksq(king_x - 1, king_y - 1)) == W_PAWN) { return true; }
        if (king_x < 7 && king_y > 0 && b.get(mksq(king_x + 1, king_y - 1)) == W_PAWN) { return true; }
    }

    // check knight
    Piece enemy_knight = white ? B_KNIGHT : W_KNIGHT;
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(king_x + XKN[i], king_y + YKN[i])) && b.get(temp) == enemy_knight) {
            return true;
        }
    }

    // check king (daft but thorough - stops legal_moves letting kings move next to each other)
    Piece enemy_king = white ? B_KING : W_KING;
    for (int i = 0; i < 8; ++i) {
        if (val(temp = mksq(king_x + XD[i], king_y + YD[i])) && b.get(temp) == enemy_king){
            return true;
        }
    }

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
    if (line_search_check(b, ksq, bishop, queen, Delta{1, 1})
         || line_search_check(b, ksq, bishop, queen, Delta{1, -1})
         || line_search_check(b, ksq, bishop, queen, Delta{-1, 1})
         || line_search_check(b, ksq, bishop, queen, Delta{-1, -1})) {
        return true;    
    }
            
    // orthogonal: rook or queen
    if (line_search_check(b, ksq, queen, rook, Delta{1, 0})
         || line_search_check(b, ksq, queen, rook, Delta{-1, 0})
         || line_search_check(b, ksq, queen, rook, Delta{0, 1})
         || line_search_check(b, ksq, queen, rook, Delta{0, -1})) {
        return true;
    }
    
    return false;
}

bool in_check_hard(const Board & b) {
    return in_check_hard(b, b.colour_to_move());
}

/*************************************************************************
 *  Legal move functions
 ************************************************************************/

void legal_moves(const Board & b, vector<Move> & moves) {
    
    Colour col = b.get_white() ? WHITE : BLACK;
    
    // get moves for every piece on the board
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Square sq = mksq(x, y);
            if (colour(b.get(sq)) == col) {
                piecemoves_ignore_check(b, sq, moves);
            }   
        }
    }
    
    // for each, check whether it leaves us in check
    
    auto itr = moves.begin();
    Move m{0, 0, 0};
    
    while (itr != moves.end()) {
        m = *itr;
        //cout << movetosan(b, m) << " ";
        Board local = b;
        local.mutate_hard(m);
        if (in_check_hard(local)) {
            itr = moves.erase(itr);
        } else {
            itr++;
        }
        
    }
    
}

vector<Move> legal_moves(const Board & b) {
    vector<Move> moves;
    legal_moves(b, moves);
    return moves;
}

