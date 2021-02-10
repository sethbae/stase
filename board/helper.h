#ifndef HELPER_H
#define HELPER_H

#include "board.h"

const Byte SHIFT_ROW = 16;
const Byte SHIFT_COL = 1;

inline Square mksq(int row, int col) {
    return (Square) ((row << 4) | col);
}

inline Square inc_row(Square s) {
    return s + SHIFT_ROW;
}

inline Square dec_row(Square s) {
    return s - SHIFT_ROW;
}

inline Square inc_col(Square s) {
    return s + SHIFT_COL;
}

inline Square dec_col(Square s) {
    return s - SHIFT_COL;
}

inline Square reset_col(Square s) {
    return s & HI4;
}

inline Square reset_row(Square s) {
    return s & LO4;
}

inline Byte get_row(Square s) {
    return s >> 4;
}

inline Byte get_col(Square s) {
    return s & LO4;
}

inline bool val_row(Square s) {
    return !(s & 128);
}

inline bool val_col(Square s) {
    return !(s & 8);
}

inline Square stosq(string str) {
    return mksq(str[1] - '1', str[0] - 'a');
}

inline string sqtos(Square sq) {
    stringstream ss;
    ss << (char) (get_col(sq) + 'a') << (char) (get_row(sq) + '1');
    return ss.str();
}

inline bool is_white(Piece p) {
    return p & 8;
}

Board empty_board() {
    
    Board b;
    
    for (Square s = mksq(0, 0) ; get_row(s) < 8; s = inc_row(s)) {
        for ( ; get_col(s) < 8; s = inc_col(s)) {
            b.set(s, EMPTY);
        }
        s = reset_col(s);
    }
    
    return b;
}


char ptoc(Piece p) {
    switch (p) {
        case B_KING:    return 'k';
        case B_QUEEN:   return 'q';
        case B_ROOK:    return 'r';
        case B_BISHOP:  return 'b';
        case B_KNIGHT:  return 'n';
        case B_PAWN:    return 'p';
        case W_KING:    return 'K';
        case W_QUEEN:   return 'Q';
        case W_ROOK:    return 'R';
        case W_BISHOP:  return 'B';
        case W_KNIGHT:  return 'N';
        case W_PAWN:    return 'P';
        default:        return '*';
    }
}

Piece ctop(char c) {
    switch (c) {
        case 'K':   return W_KING;
        case 'Q':   return W_QUEEN;
        case 'R':   return W_ROOK;
        case 'B':   return W_BISHOP;
        case 'N':   return W_KNIGHT;
        case 'P':   return W_PAWN;
        case 'k':   return B_KING;
        case 'q':   return B_QUEEN;
        case 'r':   return B_ROOK;
        case 'b':   return B_BISHOP;
        case 'n':   return B_KNIGHT;
        case 'p':   return B_PAWN;
        default:    return EMPTY;
    }
}

inline string get_word(stringstream & ss) {
    string word;
    ss >> word;
    return word; 
}

inline int get_number(stringstream & ss) {
    int num;
    ss >> num;
    return num; 
}

void fill_board(Board & b, string & arrangement) {
    int i = 0;  // point in string
    char c;     // char read
    
    /* read in pieces first */
    unsigned row = 7, col = 0;
    while ( i < (int) arrangement.size() && (c = arrangement.at(i++)) != ' ') {
        
        Piece p;
        
        if (c == '/') {
            // forward slash: move on to next row
            col = 0;
            --row;
        } else if ((p = ctop(c)) != EMPTY) {
            
            // valid piece: output it
            if (row < 8 && col < 8) {
                b.set(mksq(row, col), p);
                ++col;
            } else {
                // cout << "Illegal coords: " << row << " " << col << "\n";
                b = empty_board();
                return;
            }
            
        } else if ('1' <= c && c <= '8') {
            
            // legal integer: skip forward in row, printing empty
            col += (c - '0');

        } else {
            // illegal character
            // cout << "Illegal char\n";
            b = empty_board();
            return;
        }
        
    }

    // if x,y != 7,7 return failure
}

void fill_config(Board & b, stringstream & words) {

    string turn = get_word(words);
    string castle = get_word(words);

    b.set_white(turn == "w");

    b.set_cas_ws(castle.find('K') != string::npos);
    b.set_cas_wl(castle.find('Q') != string::npos);
    b.set_cas_bs(castle.find('k') != string::npos);
    b.set_cas_bl(castle.find('q') != string::npos);

    string enpassant = get_word(words);
    if (enpassant == "-") {
        b.set_ep_exists(false);
    } else {
        b.set_ep_exists(true);
        b.set_ep_file(get_col(stosq(enpassant)));
    }

    int halfmoves = get_number(words);
    b.set_halfmoves(halfmoves);
    
    int fullmoves = get_number(words);
    b.set_wholemoves(fullmoves);
    
}

Board fen_to_board(string fen) {
    stringstream stream(fen);

    Board b = empty_board();
    string arrangement = get_word(stream);
    fill_board(b, arrangement);

    fill_config(b, stream);
    
    return b;
}

string board_to_fen(const Board & b) {
    stringstream ss;

    // Build arrangement string
    for (int i = 7; i >= 0; --i) {
        int spaces = 0;
        for (int j = 0; j < 8; ++j) {
            char piece = ptoc(b.get(mksq(i, j)));
            if (piece == '*') {
                ++spaces;
            } else if (spaces) {
                ss << spaces << piece;
                spaces = 0;
            } else {
                ss << piece;
            }
        }

        if (spaces) 
            ss << spaces;

        if (i) 
            ss << "/";
    }

    // Append turn
    ss << " " << (b.get_white() ? "w" : "b");

    // Append castle rights
    ss << " ";
    ss << (b.get_cas_ws() ? "K" : "");
    ss << (b.get_cas_wl() ? "Q" : "");
    ss << (b.get_cas_bs() ? "k" : "");
    ss << (b.get_cas_bl() ? "q" : "");
    
    if (!(b.get_cas_ws() | b.get_cas_wl() | b.get_cas_bs() | b.get_cas_bl())) {
        ss << "-";
    }

    // Append enpassant
    string enstr = sqtos(b.get_ep_sq());
    ss << " " << (!b.get_ep_exists() ? "-" : enstr);

    // Append half and full moves
    ss << " " << b.get_halfmoves() << " " << b.get_wholemoves();

    return ss.str();
}

Board starting_pos() {
    
    Board b = empty_board();
    
    for (Square s = mksq(1, 0); val_col(s); s = inc_col(s)) {
        b.set(s, W_PAWN);
    }
    
    for (Square s = mksq(6, 0); val_col(s); s = inc_col(s)) {
        b.set(s, B_PAWN);
    }
    
    Square s = mksq(0, 0);
    b.set(s, W_ROOK);
    
    s = inc_col(s);
    b.set(s, W_KNIGHT);
    
    s = inc_col(s);
    b.set(s, W_BISHOP);
    
    s = inc_col(s);
    b.set(s, W_QUEEN);
    
    s = inc_col(s);
    b.set(s, W_KING);
    
    s = inc_col(s);
    b.set(s, W_BISHOP);
    
    s = inc_col(s);
    b.set(s, W_KNIGHT);
    
    s = inc_col(s);
    b.set(s, W_ROOK);
    
    s = mksq(7, 0);
    b.set(s, B_ROOK);
    
    s = inc_col(s);
    b.set(s, B_KNIGHT);
    
    s = inc_col(s);
    b.set(s, B_BISHOP);
    
    s = inc_col(s);
    b.set(s, B_QUEEN);
    
    s = inc_col(s);
    b.set(s, B_KING);
    
    s = inc_col(s);
    b.set(s, B_BISHOP);
    
    s = inc_col(s);
    b.set(s, B_KNIGHT);
    
    s = inc_col(s);
    b.set(s, B_ROOK);
    
    b.set_white(1);

    b.set_cas_ws(1);
    b.set_cas_wl(1);
    b.set_cas_bs(1);
    b.set_cas_bl(1);

    b.set_ep_exists(0);
    b.set_halfmoves(0);
    b.set_wholemoves(1);
    
    return b;
}

#endif