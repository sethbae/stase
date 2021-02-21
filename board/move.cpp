#include "board.h"

#include <stdint.h>
#include <string>
#include <vector>
using std::string;


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

string ptos_alg(Piece p) {
    switch (type(p)) {
        case KING: return "K";
        case QUEEN: return "Q";
        case ROOK: return "R";
        case BISHOP: return "B";
        case KNIGHT: return "N";
        case PAWN: return "";
        default: return "";
    }
}

Move santomove(string san) {
    // needs to be able to find where the piece is coming from given its destination
    Move m = {0, 0, 0};
    return m;
}

string movetosan(Board & b, Move m) {
    // doesn't do captures, checks, promotions or disambiguations
    return ptos_alg(b.get(m.from)) + sqtos(m.to);
}

void line_search(const Board & b, const Square s, 
                    void step(Square &), 
                    bool valid(const Square &)) {
    
    Piece p = b.get(s);
    Square temp = s;
    bool cont = true;
    
    step(temp);
    
    while (valid(temp) && cont) {
        Piece otherp = b.get(temp);
        if (type(otherp) == EMPTY) {
            // cout << sqtos(temp) << " ";
        } else {
            cont = false;
            if (colour(otherp) != colour(p)) {
                // cout << sqtos(temp) << "x ";
            }
        }
        step(temp);
    }
}

void ortho(const Board & b, const Square start_sq) {
    
    line_search(b, start_sq, inc_x, val_x);
    line_search(b, start_sq, dec_x, val_x);
    line_search(b, start_sq, inc_y, val_y);
    line_search(b, start_sq, dec_y, val_y);
    // cout << "\n";
    
}

void diag(const Board & b, const Square start_sq) {
    
    line_search(b, start_sq, diag_ur, val);
    line_search(b, start_sq, diag_dl, val);
    line_search(b, start_sq, diag_dr, val);
    line_search(b, start_sq, diag_ul, val);
    // cout << "\n";
    
}

void piecemoves(Board & b, Square s) {

    Piece p = b.get(s);

    switch (type(p)) {
        
        case ROOK: 
            ortho(b, s);
            break;
            
        case BISHOP: 
            diag(b, s);
            break;
            
        case QUEEN: 
            ortho(b, s);
            diag(b, s);
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

    // pr_mask(box);

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

bool is_legal_example(Move m, const Board & b) {
    Bitmap move_pattern = pattern_map(m.from, b.get(m.from));
    // pr_mask(move_pattern);
    if ((move_pattern & ~square_map(m.from) & square_map(m.to)) == 0) {
        return false;
    }

    if (b.get(m.to) != EMPTY && colour(b.get(m.to)) == colour(b.get(m.from))) {
        return false;
    }

    Ptype p = type(b.get(m.from));
    if ((p == QUEEN || p == ROOK || p == BISHOP) && !is_unobstructed(m, vacancy_map(b))) {
        return false;
    }

    // Other legality tests...

    return true;
}

void get_legal_moves_from_pos(const Board & b, Square sq, std::vector<string> & moves) {
    string piece = ptos_alg(b.get(sq));
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (is_legal_example({sq, mksq(i, j), 0}, b)) {
                moves.push_back(piece + sqtos(mksq(i, j)));
            }
        }
    }
}

void get_all_legal_moves(const Board & b, Ptype turn, std::vector<string> & all_moves) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (colour(b.get(mksq(i, j))) == turn) {
                get_legal_moves_from_pos(b, mksq(i, j), all_moves);
            }
        }
    }
}

/* int main(void) {
    Board b = fen_to_board("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
    std::vector<string> all_moves;
    get_all_legal_moves(b, WHITE, all_moves);
    pr(b);
    cout << "\n\"Legal\" moves: \n";
    int counter = 0;
    for (string s : all_moves) {
        cout << s << " ";
        if (++counter % 16 == 0) {
            cout << "\n";
        }
    }
} */


/* int main(void) {
    
    Board b = fen_to_board("8/8/8/8/8/2Q5/8/8 w - - 0 1");
    Square s = stosq("c3");
    pr(b);
    piecemoves(b, s);

} */

/* int main(void) {
    Board b = starting_pos();
    Bitmap map = gen_bitmap(b, [] (Square sq, Piece p) {
        return (get_x(sq) + get_y(sq))% 2 == 0;
    });
    pr_mask(map);
} */
