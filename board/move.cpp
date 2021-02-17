#include <stdint.h>
#include <string>
#include <vector>
using std::string;
#include "board.h"
#include "helper.h"
#include "print.h"
#include "bitmap.h"

/* defines a move data type and defines functions to make moves, and return legal moves etc */

/* 
from (8 bits)
to (8 bits)
flags (16 bits):
    - bit 0:        is_prom
    - bit 1:        is_castle
    - bit 2:        is_enpassant
    - bit 3:        is_capture

    - bit 4-5:      promotion piece
    - bit 6:        castle side
    - bit 7-9:      captured piece
 */

struct Move {
    uint_fast8_t from;
    uint_fast8_t to;
    uint_fast16_t flags;    

    bool is_prom() { return flags & 1; }
    bool is_cas() { return flags & 2; }
    bool is_ep() { return flags & 4; }
    bool is_cap() { return flags & 8; }

    Piece prom() {
        switch((flags & 6) >> 1) {
            case 1: return BISHOP;
            case 2: return KNIGHT;
            case 3: return ROOK;
            case 0:
            default: return QUEEN;
        }
    }


    int cas_side() {
        return (flags & (1 << 6)) == 0;
    }

    int ep_file() {
        return get_x(to);
    }

    Piece cap_piece() {
        return (Piece) flags & (7 << 7);
    }

};

void make_move(Board & b, Move m) {
    // can't do castles or en-passant
    Piece p = b.get(m.from);
    b.set(m.to, p);
    b.set(m.from, EMPTY);

}

string ptos_alg(Piece p) {
    switch (TYPE[p]) {
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

// Result is undefined if the move is not on a diagonal or orthogonal line, eg. a knight's move
/* bool is_obstructed(const Board & b, Move m) {
    int x_offset = get_x(m.to) > get_x(m.from) ? 1 : get_x(m.to) < get_x(m.from) ? -1 : 0;
    int y_offset = get_y(m.to) > get_y(m.from) ? 1 : get_y(m.to) < get_y(m.from) ? -1 : 0;
    
    int x = get_x(m.from) + x_offset;
    int y = get_y(m.from) + y_offset;

    while (get_y(m.to) != y || get_x(m.to) != x) {
        if (TYPE[b.get(mksq(x, y))] != EMPTY) {
            return true;
        }

*/

void line_search(const Board & b, const Square s, 
                    void step(Square &), 
                    bool valid(const Square &)) {
    
    Piece p = b.get(s);
    Square temp = s;
    bool cont = true;
    
    step(temp);
    
    while (valid(temp) && cont) {
        Piece otherp = b.get(temp);
        if (TYPE[otherp] == EMPTY) {
            cout << sqtos(temp) << " ";
        } else {
            cont = false;
            if (COLOUR[otherp] != COLOUR[p]) {
                cout << sqtos(temp) << "x ";
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
    cout << "\n";
    
}

void diag(const Board & b, const Square start_sq) {
    
    line_search(b, start_sq, diag_ur, val);
    line_search(b, start_sq, diag_dl, val);
    line_search(b, start_sq, diag_dr, val);
    line_search(b, start_sq, diag_ul, val);
    cout << "\n";
    
}

void piecemoves(Board & b, Square s) {

    Piece p = b.get(s);

    switch (TYPE[p]) {
        
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
bool is_unobstructed(Move m, uint64_t vacancy) {
    int xs = get_x(m.from), ys = get_y(m.from), xe = get_x(m.to), ye = get_y(m.to);

    uint64_t v_range, h_range;          // To account for pieces going backwards...
    if (xs > xe) {
        h_range = gen_mcol(xe, xs);
    } else {
        h_range = gen_mcol(xs, xe);
    }

    if (ys > ye) {
        v_range = gen_mrow(ye, ys);
    } else {
        v_range = gen_mrow(ys, ye);
    }

    // The box contains only the squares within the smallest square containing both the start and end square
    uint64_t box = v_range & h_range;

    // pos is the representation of both the start and end squares only
    uint64_t pos = gen_pos(m.from) | gen_pos(m.to);

    // The only straight line connecting both squares (needs optimisation imo)
    uint64_t line;
    int diffx = xe - xs;
    int diffy = ye - ys;
    if (diffx == 0) {               // if it's vertical
        line = gen_file(xe);
    } else if (diffy == 0) {        // if it's horizontal
        line = gen_row(ye);
    } else if (diffy == diffx) {    // if it's a positive diagonal
        line = gen_xy(m.from);
    } else {                        // if it's a negative diagonal
        line = gen_nxy(m.to);
    }
    
    uint64_t path = box & line & ~pos;
    // pr_mask(box);

    // The path between a sqaure and another would be unobstructed only if there is no occupied square in between them
    return (path & vacancy) == path;

}

bool is_legal_example(Move m, const Board & b) {
    uint64_t move_pattern = gen_pattern(m.from, b.get(m.from));
    // pr_mask(move_pattern);
    if ((move_pattern & ~gen_pos(m.from) & gen_pos(m.to)) == 0) {
        return false;
    }

    if (b.get(m.to) != EMPTY && COLOUR[b.get(m.to)] == COLOUR[b.get(m.from)]) {
        return false;
    }

    Ptype p = TYPE[b.get(m.from)];
    if ((p == QUEEN || p == ROOK || p == BISHOP) && !is_unobstructed(m, gen_vacancy_map(b))) {
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

void get_all_legal_moves(const Board & b, Ptype colour, std::vector<string> & all_moves) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (COLOUR[b.get(mksq(i, j))] == colour) {
                get_legal_moves_from_pos(b, mksq(i, j), all_moves);
            }
        }
    }
}

/* int main(void) {
    Board b = starting_pos();
    std::vector<string> all_moves;
    get_all_legal_moves(b, WHITE, all_moves);
    pr(b);
    cout << "\n\"Legal\" moves: ";
    for (string s : all_moves) {
        cout << s << " ";
    }
} */


/*int main(void) {
    
    Board b = fen_to_board("8/8/8/8/8/2Q5/8/8 w - - 0 1");
    Square s = stosq("c3");
    pr(b);
    piecemoves(b, s);

}*/
