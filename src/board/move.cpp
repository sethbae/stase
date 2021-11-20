#include <string>
#include <vector>
using std::vector;
using std::string;
#include <iostream>
using std::cout;

#include "board.h"

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
 *  - bit 7-10:      captured piece
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

const int knight_dirs[8][2] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};

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
Piece Move::get_prom_piece(Ptype col) const {
    Piece p = (col == WHITE) ? W_QUEEN : B_QUEEN;
    return p + get_prom_shift();
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

Move empty_move() {
    Move m = {0, 0, 0, 0, 0};
    return m;
}

/*************************************************************************
 *  VECTOR piece move functions
 ************************************************************************/

void line_search(const Board & b, const Square s,
                    void step(Square &), 
                    bool (* valid)(const Square &),
                    vector<Move> & moves) {
        
    Piece p = b.get(s);
    Square temp = s;
    bool cont = true;
    Move m = empty_move();
    m.from = s;
    
    step(temp);
    
    while ((*valid)(temp) && cont) {
        //cout << sqtos(temp);
        Piece otherp = b.get(temp);
        if (type(otherp) == EMPTY) {
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
        step(temp);
    }
    
}

void ortho(const Board & b, const Square start_sq, vector<Move> & moves) {
   
    line_search(b, start_sq, inc_x, val_x, moves); 
    line_search(b, start_sq, dec_x, val_x, moves);
    line_search(b, start_sq, inc_y, val_y, moves);
    line_search(b, start_sq, dec_y, val_y, moves);
    
}

void diag(const Board & b, const Square start_sq, vector<Move> & moves) {
    
    line_search(b, start_sq, diag_ur, val, moves);
    line_search(b, start_sq, diag_dl, val, moves);
    line_search(b, start_sq, diag_dr, val, moves);
    line_search(b, start_sq, diag_ul, val, moves);
    
}

void knight_moves(const Board & b, const Square s, vector<Move> & moves) {

    int x = get_x(s), y = get_y(s);
    Ptype knightcol = colour(b.get(s));
    Square sq;
    
    Move m = empty_move();
    m.from = s;

    // for (int i = 0; i < 8; ++i) {
    //     if (val(sq = mksq(x + knight_dirs[i][0], y + knight_dirs[i][1])) && colour(b.get(sq)) != knightcol) {
    //         m.to = sq;
    //         if (type(b.get(sq)) != EMPTY) {
    //             m.set_cap();
    //             m.set_cap_piece(b.get(sq));
    //         }
    //         moves.push_back(m);
    //         m.unset_cap();
    //     }
    // }
    
    if (val(sq = mksq(x + 1, y + 2)) && colour(b.get(sq)) != knightcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x + 1, y - 2)) && colour(b.get(sq)) != knightcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x + 2, y + 1)) && colour(b.get(sq)) != knightcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x + 2, y - 1)) && colour(b.get(sq)) != knightcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x - 1, y + 2)) && colour(b.get(sq)) != knightcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x - 1, y - 2)) && colour(b.get(sq)) != knightcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x - 2, y + 1)) && colour(b.get(sq)) != knightcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x - 2, y - 1)) && colour(b.get(sq)) != knightcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }

}

// checks the kings castling squares are empty and check-free: nothing more
bool castle_checks(Board b, const Ptype col, const bool kingside) {
    
    Square s1, s2, s3; // intermediate squares
    Square k = (col == WHITE) ? mksq(4, 0) : mksq(4, 7); // king starting square
        
    if (col == WHITE && kingside) {
        
        s1 = mksq(5, 0);
        s2 = mksq(6, 0);
        
        if (b.get(s1) == EMPTY && b.get(s2) == EMPTY) {
            
            b.set(k, EMPTY);
            b.set(s1, W_KING);
            
            if (!in_check_hard(b, WHITE)) {
                b.set(s1, EMPTY);
                b.set(s2, W_KING);
                
                return !in_check_hard(b, WHITE);
            }
            
        }           
        
    } else if (col == WHITE && !kingside) {
        
        s1 = mksq(3, 0);
        s2 = mksq(2, 0);
        s3 = mksq(1, 0);
        
        if (b.get(s1) == EMPTY && b.get(s2) == EMPTY && b.get(s3) == EMPTY) {
            
            b.set(k, EMPTY);
            b.set(s1, W_KING);
            
            if (!in_check_hard(b, WHITE)) {
                b.set(s1, EMPTY);
                b.set(s2, W_KING);
                
                return !in_check_hard(b, WHITE);
            }
            
        }   
                
    } else if (col == BLACK && kingside) {
        
        s1 = mksq(5, 7);
        s2 = mksq(6, 7);
        
        if (b.get(s1) == EMPTY && b.get(s2) == EMPTY) {
            
            b.set(k, EMPTY);
            b.set(s1, B_KING);
            
            if (!in_check_hard(b, BLACK)) {
                b.set(s1, EMPTY);
                b.set(s2, B_KING);
                
                return !in_check_hard(b, BLACK);
            }
            
        }
                  
    } else { // BLACK and !kingside
        
        s1 = mksq(3, 7);
        s2 = mksq(2, 7);
        s3 = mksq(1, 7);
        
        if (b.get(s1) == EMPTY && b.get(s2) == EMPTY && b.get(s3) == EMPTY) {
            
            b.set(k, EMPTY);
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
    
    Ptype kingcol = colour(b.get(s));
    Square sq;
    Move m = empty_move();
    m.from = s;
    
    // check castle for white
    if (kingcol == WHITE && !in_check_hard(b, WHITE)) {
        
        Move c = empty_move();
        c.from = s;
        
        if (b.get_cas_ws() && castle_checks(b, WHITE, true)) {
            c.to = mksq(6, 0);
            c.set_cas();
            c.set_cas_short();
            moves.push_back(c);
        }
        
        if (b.get_cas_wl() && castle_checks(b, WHITE, false)) {
            c.to = mksq(2, 0);
            c.set_cas();
            c.unset_cas_short();
            moves.push_back(c);
        }
    
    // and for black
    } else if (kingcol == BLACK && !in_check_hard(b, BLACK)) {
        
        Move c = empty_move();
        c.from = s;
        
        if (b.get_cas_bs() && castle_checks(b, BLACK, true)) {
            c.to = mksq(6, 7);
            c.set_cas();
            c.set_cas_short();
            moves.push_back(c);
        }
        
        if (b.get_cas_bl() && castle_checks(b, BLACK, false)) {
            c.to = mksq(2, 7);
            c.set_cas();
            c.unset_cas_short();
            moves.push_back(c);
        }
        
    }
    
    // check all adjacent squares

    // int adj[] = {-1, 0, 1};
    // for (int i = 0; i < 3; ++i) {
    //     for (int j = 0; j < 3; ++j) {
    //         if (val(sq = mksq(adj[i], adj[j])) && colour(b.get(sq)) != kingcol) {
    //             m.to = sq;
    //             if (type(b.get(sq)) != EMPTY) {
    //                 m.set_cap();
    //                 m.set_cap_piece(b.get(sq));
    //             }
    //             moves.push_back(m);
    //             m.unset_cap();
    //         }
    //     }
    // }



    int x = get_x(s), y = get_y(s);
    if (val(sq = mksq(x + 1, y + 1)) && colour(b.get(sq)) != kingcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x + 1, y)) && colour(b.get(sq)) != kingcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x + 1, y - 1)) && colour(b.get(sq)) != kingcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x, y + 1)) && colour(b.get(sq)) != kingcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x, y - 1)) && colour(b.get(sq)) != kingcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x - 1, y + 1)) && colour(b.get(sq)) != kingcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x - 1, y)) && colour(b.get(sq)) != kingcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }
    
    if (val(sq = mksq(x - 1, y - 1)) && colour(b.get(sq)) != kingcol) {
        m.to = sq;
        if (type(b.get(sq)) != EMPTY) {
            m.set_cap();
            m.set_cap_piece(b.get(sq));
        }
        moves.push_back(m);
        m.unset_cap();
    }

}

void pawn_moves(const Board & b, const Square s, vector<Move> & moves) {
       
    int x = get_x(s), y = get_y(s);
    Ptype pawncolour = colour(b.get(s));
    Square sq;
    
    Move m = empty_move();
    m.from = s;
    
    int FORWARD;
    int START_RANK;
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
            m.set_cap_piece((pawncolour == WHITE) ? B_PAWN : W_PAWN);
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
            
        case KING: {
            king_moves(b, s, moves);
            break;
        }
            
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

bool line_search_check(const Board & b, Square sq, const Piece p1, const Piece p2,
                        void step(Square &),
                        bool valid(const Square &)) {
                                
    step(sq);
    
    while (valid(sq)) {

        Piece otherp = b.get(sq);
        
        if (type(otherp) != EMPTY) {  
            return otherp == p1 || otherp == p2;
        }
        step(sq);
    }
    
    return false;                        
                        
}

/* returns true iff the player of the given colour is in check */
bool in_check_hard(const Board & b, Ptype col) {
    
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

    // for (int i = 0; i < 8; ++i) {
    //     if (val(sq = mksq(king_x + knight_dirs[i][0], king_y + knight_dirs[i][1])) && b.get(sq) == enemy_knight) {
    //         return true;
    //     }
    // }


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

    // check king (daft but thorough - stops legal_moves letting kings move next to each other)
    Piece enemy_king = white ? B_KING : W_KING;

    // int adj[] = {-1, 0, 1};
    // for (int i = 0; i < 3; ++i) {
    //     for (int j = 0; j < 3; ++j) {
    //         if (val(sq = mksq(king_x + adj[i], king_y + adj[j]))  && b.get(sq) == enemy_king){
    //             return true;
    //         }
    //     }
    // }


    if (val(sq = mksq(king_x + 1, king_y + 1))  && b.get(sq) == enemy_king)
        return true;
    if (val(sq = mksq(king_x + 1, king_y))      && b.get(sq) == enemy_king)
        return true;
    if (val(sq = mksq(king_x + 1, king_y - 1))  && b.get(sq) == enemy_king)
        return true;
    if (val(sq = mksq(king_x,     king_y + 1))  && b.get(sq) == enemy_king)
        return true;
    if (val(sq = mksq(king_x,     king_y - 1))  && b.get(sq) == enemy_king)
        return true;
    if (val(sq = mksq(king_x - 1, king_y + 1))  && b.get(sq) == enemy_king)
        return true;
    if (val(sq = mksq(king_x - 1, king_y))      && b.get(sq) == enemy_king)
        return true;
    if (val(sq = mksq(king_x - 1, king_y - 1))  && b.get(sq) == enemy_king)
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

bool in_check_hard(const Board & b) {
    return in_check_hard(b, b.colour_to_move());
}

/*************************************************************************
 *  Legal move functions
 ************************************************************************/

void legal_piecemoves(const Board & b, const Square s, vector<Move> & moves) {
    b.get(s); moves.size();
}

void legal_moves(const Board & b, vector<Move> & moves) {
    
    Ptype col = b.get_white() ? WHITE : BLACK;
    
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

