#include <iostream>
using std::cout;
#include <stdint.h>
#include <string>
#include <sstream>
using std::stringstream;
using std::string;
#include <vector>
using std::vector;

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
 *      -is_white():    returns true iff the piece is white
 *      -ptoc():        returns a character depicting the piece (K, q etc)
 *
 *  Implementation details:
 *  
 *  Board:
 *      -> 4 bits per square of board
 *      -> the board struct stores a 2d array of bytes
 *      -> the config word is a 32 bits
 *      - Config bits:
 *          Bit 0:          Turn colour
 *          Bit 1 - 4:      Castling Rights
 *          Bit 5 - 8:      En passant
 *          Bit 9 - 15:     Half move counter
 *          Bit 16 - 31:    Full move counter
 *      
 *  Square:
 *      -> 8 bits per square
 *      -> 4 high bits store one coordinate (0-8 inc)
 *      -> 4 low bits store another (0-8 inc)
 *
 *  Piece:
 *      -> enum of type 'Byte', although only 4 (low) bits are used
 *      -> White pieces have the MSB set.
 *      -> EMPTY is white, as it goes (value 15).
 *      
 *  Note: To decide which half of a byte is addressed, we use even/oddness.
 *          So for a 3 bit index n, n/2 is used to get the byte (i.e. n >> 1),
 *          and then n % 2 == 0 is used to decide between high and low (i.e. n & 1).
 *      
 *****************************************************************************/

const unsigned LO4 = 15;
const unsigned HI4 = 240;
const unsigned LO3 = 7;
const unsigned HI3 = 112;

typedef uint_fast8_t Byte;
typedef uint_fast32_t Int;
typedef uint_fast8_t Square;

enum Ptype {

    /* defined values: so that we can translate both ways */
    B_KING = 0,
    B_QUEEN = 1,
    B_ROOK = 2,
    B_KNIGHT = 3,
    B_BISHOP = 4,
    B_PAWN = 5,
    
    W_KING = 8,
    W_QUEEN = 9,
    W_ROOK = 10,
    W_KNIGHT = 11,
    W_BISHOP = 12,
    W_PAWN = 13,
    
    EMPTY = 14,
    
    /* other values which appear in the lookup tables */
    KING,
    QUEEN,
    ROOK,
    KNIGHT,
    BISHOP,
    PAWN,
    
    BLACK,
    WHITE,
    
    INVALID
};

/* lookup tables for querying information about a given piece. The 4-bit piece (0-14 valid)
   is used as an index into the relevant table */

const Ptype PIECE[] = {
            B_KING, B_QUEEN, B_ROOK, B_KNIGHT, B_BISHOP, B_PAWN,        // indexes 0-5
            INVALID, INVALID,                                           // indexes 6, 7
            W_KING, W_QUEEN, W_ROOK, W_KNIGHT, W_BISHOP, W_PAWN,        // indexes 8-13
            EMPTY                                                       // index 14
      };
                        
const Ptype TYPE[] = {
            KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN,    // indexes 0-5
            INVALID, INVALID,                           // indexes 6, 7
            KING, QUEEN, ROOK, KNIGHT, BISHOP, PAWN,    // indexes 8-13
            EMPTY                                       // index 14
      };
                       
const Ptype COLOUR[] = {
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,   // indexes 0-5
            INVALID, INVALID,                           // indexes 6, 7
            WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,   // indexes 8-13
            EMPTY                                       // index 14
      };

typedef Byte Piece;

struct Conf {
    bool w : 1;
    bool K : 1;
    bool Q : 1;
    bool k : 1;
    bool q : 1;
    bool ep : 1;
    unsigned epfile : 3;
    unsigned half_moves : 6;
    unsigned whole_moves : 17;
};

const unsigned WHITE_MASK = 1 << 0;
const unsigned CAS_WS_MASK = 1 << 1;
const unsigned CAS_WL_MASK = 1 << 2;
const unsigned CAS_BS_MASK = 1 << 3;
const unsigned CAS_BL_MASK = 1 << 4;
const unsigned EP_EX_MASK = 1 << 5;
const unsigned EP_FILE_MASK = 7 << 6;
const unsigned HALF_M_MASK = 127 << 9;
const unsigned WHOLE_M_MASK = (~0) << 16;

struct Board {

    // std::array<std::array<Byte, 4>, 8> squares;
    Byte squares[8][4];
    Int conf;

    /* read 4 bits from given square address */
    Piece get(Square sq) {
        Byte b = squares[sq >> 4][(sq & LO3) >> 1]; // use high as 0-7, low/2 as 0-3
        return (Piece) ((sq & 1) ? (b & LO4) : (b >> 4)); // depending on parity, read 4 bits
    }

    /* write 4 bits to given square address */
    void set(Square sq, Piece val) {
        
        Byte ind1 = sq >> 4;    // calculate correct indices as above
        Byte ind2 = (sq & LO3) >> 1;
        
        if (sq & 1) {
             // if odd, write to high
            squares[ind1][ind2] = (squares[ind1][ind2] & HI4) | val;
        } else {
            // if even, write to low
            squares[ind1][ind2] = (val << 4) | (squares[ind1][ind2] & LO4);
        }
        
    }
    
    /* get/set turn (who to play) */
    inline bool get_white() { return conf & WHITE_MASK; }
    inline void set_white(bool b) {
        if (b)
            conf |= WHITE_MASK;
        else 
            conf &= ~WHITE_MASK;
    }
    inline void flip_white() { conf ^= WHITE_MASK; }
    
    /* get/set castling rights individually */
    inline bool get_cas_ws() { return conf & CAS_WS_MASK; }    
    inline void set_cas_ws(bool b) {         
        if (b)
            conf |= CAS_WS_MASK;
        else 
            conf &= ~CAS_WS_MASK; 
    }
    inline bool get_cas_wl() { return conf & CAS_WL_MASK; }    
    inline void set_cas_wl(bool b) {         
        if (b)
            conf |= CAS_WL_MASK;
        else 
            conf &= ~CAS_WL_MASK; 
    }
    inline bool get_cas_bs() { return conf & CAS_BS_MASK; }    
    inline void set_cas_bs(bool b) {         
        if (b)
            conf |= CAS_BS_MASK;
        else 
            conf &= ~CAS_BS_MASK; 
    }
    inline bool get_cas_bl() { return conf & CAS_BL_MASK; }    
    inline void set_cas_bl(bool b) {         
        if (b)
            conf |= CAS_BL_MASK;
        else 
            conf &= ~CAS_BL_MASK; 
    }
    
    /* get/set en-passant boolean */
    inline bool get_ep_exists() { return conf & EP_EX_MASK; }
    inline void set_ep_exists(bool b) { 
        if (b)
            conf |= EP_EX_MASK;
        else
            conf &= ~EP_EX_MASK;
    }
    
    /* read write 3 bits representing ep file */
    inline unsigned get_ep_file() {
        return (conf & EP_FILE_MASK) >> 6;
    }
    inline void set_ep_file(unsigned u) {
        conf = (conf & ~EP_FILE_MASK) | (u << 6);
    }
    // for convenience, return the actual square
    inline Square get_ep_sq() {
        Square mksq(int, int);
        return mksq(get_white() ? 5 : 2, get_ep_file());
    }
    
    /* read write 7 bits for the half move count */
    inline unsigned get_halfmoves() {
        return (conf & HALF_M_MASK) >> 9;
    }
    inline void set_halfmoves(unsigned u) {
        conf = (conf & ~HALF_M_MASK) | (u << 9);
    }    
    
    /* read write 16 bits for the whole move count */
    inline unsigned get_wholemoves() {
        return (conf & WHOLE_M_MASK) >> 16;
    }
    inline void set_wholemoves(unsigned u) {
        conf = (conf & ~WHOLE_M_MASK) | (u << 16);
    }
    
    
    /******************This is an XOR version of get/set epfile, half and whole. Comparable speed
    /* read write 3 bits representing ep file 
    inline unsigned get_ep_file() {
        return (conf & EP_FILE_MASK) >> 6;
    }
    inline void set_ep_file(unsigned u) {
        unsigned c = conf ^ (u << 6);
        conf ^= (c & EP_FILE_MASK);
    }
    // for convenience, return the actual square
    inline Square get_ep_sq() {
        Square mksq(int, int);
        return mksq(get_white() ? 5 : 2, get_ep_file());
    }*/

    /* read write 7 bits for the half move count 
    inline unsigned get_halfmoves() {
        return (conf & HALF_M_MASK) >> 9;
    }
    inline void set_halfmoves(unsigned u) {
        unsigned c = conf ^ (u << 9);
        conf ^= (c & HALF_M_MASK);
    }
    
    /* read write 16 bits for the half move count 
    inline unsigned get_wholemoves() {
        return (conf & WHOLE_M_MASK) >> 16;
    }
    inline void set_wholemoves(unsigned u) {
        unsigned c = conf ^ (u << 16);
        conf ^= (c & WHOLE_M_MASK);
    } */

};

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

/* print out the literal integer values of the data at each square on the board */
void pr_raw(Board b) {

    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j <=7; ++j) {
        
            int x = b.get(mksq(i, j));
            
            if (x < 10) {
                cout << " " << x << " ";
            } else {
                cout << x << " ";
            }
            
        }
        cout << "\n";
    }
    
}

/* print out a human readable chess representation of the board */
void pr_indent(Board & b, string indent) {
    for (int i = 7; i >= 0; --i) {
        cout << indent;
        for (int j = 0; j < 8; ++j) {
            Piece p = b.get(mksq(i, j));
            cout << ptoc(p) << " ";
        }
        cout << "\n";
    }
}

void pr_config(Board b) {

    cout << "Raw config: ";
    
    int* raw_bin = (int*) &b.conf;
    
    for (int i = 31; i >= 0; --i) {
        if (*raw_bin & (1 << i)) {
            cout << "1";
        } else {
            cout << "0";
        }

        if (i == 0 || i == 1 || i == 5 || i == 9 || i == 15) {
            cout << " ";
        }
    }
    cout << "\n";
    cout << "Turn: " << (b.get_white() ? "White" : "Black") << "\n";

    cout << "Castle Rights: ";
    if (b.get_cas_ws()) cout << "K ";
    if (b.get_cas_wl()) cout << "Q ";
    if (b.get_cas_bs()) cout << "k ";
    if (b.get_cas_bl()) cout << "q ";
    
    if (!(b.get_cas_ws() | b.get_cas_wl() | b.get_cas_bs() | b.get_cas_bl())) {
        cout << " -";
    }

    cout << "\n";

    if (b.get_ep_exists()) {
        cout << "Enpassant: " << sqtos(b.get_ep_sq()) << "\n";
    } else {
        cout << "Enpassant: -\n";
    }

    cout << "Half moves: " << b.get_halfmoves() << "\n";
    cout << "Full moves: " << b.get_wholemoves() << "\n";
}

void pr(Board b) {
    pr_indent(b, "");
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

void raw(Board b) {
    cout << "Raw: ";
    
    int* raw_bin = (int*) &b.conf;
    
    for (int i = 31; i >= 0; --i) {
        if (*raw_bin & (1 << i)) {
            cout << "1";
        } else {
            cout << "0";
        }

        if (i == 0 || i == 1 || i == 5 || i == 9 || i == 16) {
            cout << " ";
        }
    }
    cout << "\n";
}

void fill_config(Board & b, stringstream & words) {

    string turn = get_word(words);
    string castle = get_word(words);

    b.set_white(turn == "w");

    b.set_cas_ws(castle.find('K') != -1);
    b.set_cas_wl(castle.find('Q') != -1);
    b.set_cas_bs(castle.find('k') != -1);
    b.set_cas_bl(castle.find('q') != -1);

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

/* work in progress - doesn't read the config word */
Board fen_to_board(string fen) {
    stringstream stream(fen);

    Board b = empty_board();
    string arrangement = get_word(stream);
    fill_board(b, arrangement);

    fill_config(b, stream);
    
    return b;
}

string board_to_fen(Board & b) {
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

void pr_board_config(Board & b, string indent) {
    for (int i = 7; i >= 0; --i) {
        cout << indent;
        for (int j = 0; j < 8; ++j) {
            Piece p = b.get(mksq(i, j));
            cout << ptoc(p) << " ";
        }

        switch (i) {
            case 7: {
                cout << "\tFEN: " << board_to_fen(b);
                break;
            }

            case 6: {
                cout << "\tRaw config: ";
                int* raw_bin = (int*) &b.conf;
                for (int i = 31; i >= 0; --i) {
                    if ( *raw_bin & (1 << i)) {
                        cout << "1";
                    } else {
                        cout << "0";
                    }

                    if (i == 0 || i == 1 || i == 5 || i == 9 || i == 15) {
                        cout << " ";
                    }
                }
                break;
            }

            case 5: {
                cout << "\tTurn: " << (b.get_white() ? "White" : "Black");
                break;
            }

            case 4: {
                cout << "\tCastle Rights: ";
                if (b.get_cas_ws()) cout << "K";
                if (b.get_cas_wl()) cout << "Q";
                if (b.get_cas_bs()) cout << "k";
                if (b.get_cas_bl()) cout << "q";
                if (!(b.get_cas_ws() | b.get_cas_wl() | b.get_cas_bs() | b.get_cas_wl())) {
                    cout << "-";
                }
                
                break;  
            }

            case 3: {
                if (b.get_ep_exists()) {
                    cout << "\tEnpassant: " << sqtos(b.get_ep_sq());
                } else {
                    cout << "\tEnpassant: -";
                }
                break;
            }

            case 2: {
                cout << "\tHalf moves: " << b.get_halfmoves();
                break;
            }

            case 1: {
                cout << "\tFull moves: " << b.get_wholemoves();
                break;
            }
        }

        cout << "\n";
    }
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




/* int main() {

    vector<string> test_fens = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkb1r/pp1p1ppp/5n2/2p5/4p3/1N4P1/PPPPPPBP/RNBQK2R b KQkq - 1 5",
        "r4rk1/ppB2pp1/4p1p1/2P3q1/4Pn2/P1N2n2/2B2PPP/2R2RK1 w - - 0 24",
        "7k/1p6/p1p3p1/7p/1P2Q2P/P5P1/5r1K/5q2 w - h6 4 47",
        "2r2rk1/1p2bppp/p2pbn2/q1N1p3/2P1P3/N3BP2/PP2B1PP/2RR2K1 w - - 0 17"
    };
    
    for (int i = 0; i < 5; ++i) {
        Board b = fen_to_board(test_fens[i]);
        pr(b);
        cout << "\n";
        cout << "FEN: " << test_fens[i] << "\n";
        pr_config(b);
        cout << "\n";

        // cout << get_row(mksq("f3")) + 0;
    }
    
    return 0;
} */
