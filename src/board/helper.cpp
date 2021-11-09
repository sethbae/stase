#include "board.h"

#include <iostream>
using std::cout;
#include <ostream>
using std::ostream;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <sstream>
using std::stringstream;


/*************************************************************************************
 CONVERSIONS        for squares, FENs, SAN, pieces
 ************************************************************************************/

string sqtos(Square sq) {
    stringstream ss;
    ss << (char) (get_x(sq) + 'a') << (char) (get_y(sq) + '1');
    return ss.str();
}

/***** piece to string or char and vice versa       W_KING <----> 'K'           *****/

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
        default:        return '.';
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

// for algebraic notation (SAN), even black pieces use capitals: B_KNIGHT <----> "N"
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

/***** Move to SAN and SAN to move *****/

Move stom(const string san) {
    san.size();
    Move m = {0, 0, 0};
    return m;
}

/**
 * Converts a move to SAN. This depends on the board, which you must therefore provide.
 * The board must represent the position before the given move has been played.
 */
string mtos(const Board & b, const Move m) {

    if (is_sentinel(m)) {
        return "no move";
    }
    if (m.from == empty_move().from && m.to == empty_move().to) {
        return "empty move";
    }
    if (m.from == m.to) {
        return "invalid move";
    }

    string s = "";
    Ptype piece_colour = colour(b.get(m.from));

    bool castle = (type(b.get(m.from)) == KING && abs(get_x(m.to) - get_x(m.from)) == 2);
    bool capture = (b.get(m.to) != EMPTY);
    bool prom = (type(b.get(m.from)) == PAWN && (get_y(m.to) == 7 || get_y(m.to) == 0));

    if (!castle) {
    
        // add character for piece type
        s += ptos_alg(b.get(m.from));
        
        // for captures add x
        if (capture) {
            if (prom || type(b.get(m.from)) == PAWN) {
                s += (char) (get_x(m.from) + 'a');
            }
            s += "x";
        }
            
        // add destination
        s += sqtos(m.to);
        
        // for promotions, add =Q/R/N/B
        if (prom) {
            s += "=";
            s += ptos_alg(m.get_prom_piece(piece_colour));
        }
    
    } else {
        bool is_cas_short = m.is_cas_short() || get_x(m.to) == 6;
        s = is_cas_short ? "O-O" : "O-O-O";
    }
    
    // add + for check
    Ptype opposite_colour = (piece_colour == WHITE) ? BLACK : WHITE;
    if (in_check_hard(b, opposite_colour)) {
        s += "+";
    }
    
    return s;
}

/***** useful for reading ints and strings from a stringstream *****/
string get_word(stringstream & ss) {
    string word;
    ss >> word;
    return word; 
}

int get_number(stringstream & ss) {
    int num;
    ss >> num;
    return num; 
}

/***** FEN to board and board to FEN *****/

Board fen_to_board(const string & fen) {
    
    stringstream stream(fen);
    Board b = empty_board();
    
    /* take the first token: the pieces on the board */
    string piece_layout = get_word(stream);
    unsigned i = 0;  // point in string
    unsigned y = 7, x = 0; // coordinates on board
    while (i < piece_layout.size()) {
        
        // read the next character
        char c = piece_layout.at(i++);
        Piece p;
        
        if (c == '/') {
            // forward slash: move on to next row
            x = 0;
            --y;
        } else if ((p = ctop(c)) != EMPTY) {
            
            // valid piece: output it
            if (y < 8 && x < 8) {
                b.set(mksq(x, y), p);
                ++x;
            } else {
                return empty_board();
            }
            
        } else if ('1' <= c && c <= '8') {
            
            // legal integer: skip forward in row, printing empty
            x += (c - '0');

        } else {
            // illegal character
            return empty_board();
        }
        
    }
    
    /* now use the remaining words to get the config info */
    
    // whose turn
    b.set_white(get_word(stream) == "w");
    
    // castling rights
    string castle = get_word(stream);
    b.set_cas_ws(castle.find('K') != string::npos);
    b.set_cas_wl(castle.find('Q') != string::npos);
    b.set_cas_bs(castle.find('k') != string::npos);
    b.set_cas_bl(castle.find('q') != string::npos);

    // en passant
    string ep = get_word(stream);
    if (ep == "-") {
        b.set_ep_exists(false);
    } else {
        b.set_ep_exists(true);
        b.set_ep_file(get_x(stosq(ep)));
    }

    // half and whole moves
    b.set_halfmoves(get_number(stream));
    b.set_wholemoves(get_number(stream));
    
    return b;
}

string board_to_fen(const Board & b) {
    
    stringstream ss;

    // Build arrangement string
    for (int y = 7; y >= 0; --y) {
        int spaces = 0;
        for (int x = 0; x < 8; ++x) {
            char piece = ptoc(b.get(mksq(x, y)));
            if (piece == '.') {
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

        if (y)
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

// read PGN into a vector of the moves (still just as strings)
vector<string> read_pgn(const string & s) {
    vector<string> v;
    v.push_back(s);
    return v;
    
}

/***** useful functions for getting the start or empty boards without knowing the FENs *****/

Board starting_pos() {
    return fen_to_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

std::string starting_fen() {
    return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

Board empty_board() {
    
    Board b;
    
    for (Square s = mksq(0, 0) ; val_y(s); inc_y(s)) {
        for ( ; val_x(s); inc_x(s)) {
            b.set(s, EMPTY);
        }
        reset_x(s);
    }

    b.conf = 0;
    
    return b;
}

/*************************************************************************************
 WRITING FUNCTIONS         for board, bitmap                write to output stream
 ************************************************************************************/

void wr_board(const Board & b, const string & indent, ostream & output) {

    for (int y = 7; y >= 0; --y) {
        output << indent;
        for (int x = 0; x < 8; ++x) {
            Piece p = b.get(mksq(x, y));
            output << ptoc(p) << " ";
        }
        output << "\n";
    }
    
    output << "\n";
    return;

}

void wr_board(const Board & b, ostream & output) {
    wr_board(b, "", output);
}

void wr_board_conf(const Board & b, const string & indent, ostream & output) {
    
    for (int y = 7; y >= 0; --y) {
    
        // print the actual board layout
        output << indent;
        for (int x = 0; x < 8; ++x) {
            Piece p = b.get(mksq(x, y));
            output << ptoc(p) << " ";
        }
        
        // depending on rank, print varying pieces of config information
        switch (y) {
            
            // print the FEN
            case 7: {
                output << "\tFEN: " << board_to_fen(b);
                break;
            }
            
            // raw (binary) config string
            case 6: {
                output << "\tRaw config: ";
                int* raw_bin = (int*) &b.conf;
                for (int i = 31; i >= 0; --i) {
                    if ( *raw_bin & (1 << i)) {
                        output << "1";
                    } else {
                        output << "0";
                    }

                    if (i == 0 || i == 1 || i == 5 || i == 9 || i == 15) {
                        output << " ";
                    }
                }
                break;
            }

            // whose turn it is
            case 5: {
                output << "\tTurn: " << (b.get_white() ? "White" : "Black");
                break;
            }

            // castling rights
            case 4: {
                output << "\tCastle Rights: ";
                if (b.get_cas_ws()) output << "K";
                if (b.get_cas_wl()) output << "Q";
                if (b.get_cas_bs()) output << "k";
                if (b.get_cas_bl()) output << "q";
                if (!(b.get_cas_ws() | b.get_cas_wl() | b.get_cas_bs() | b.get_cas_wl())) {
                    output << "-";
                }
                
                break;  
            }

            // en passant
            case 3: {
                if (b.get_ep_exists()) {
                    output << "\tEnpassant: " << sqtos(b.get_ep_sq());
                } else {
                    output << "\tEnpassant: -";
                }
                break;
            }

            // half move count
            case 2: {
                output << "\tHalf moves: " << b.get_halfmoves();
                break;
            }

            // whole move count
            case 1: {
                output << "\tFull moves: " << b.get_wholemoves();
                break;
            }
        }

        output << "\n";
    }
}

void wr_board_conf(const Board & b, ostream & output) {
    wr_board_conf(b, "", output);
}

/* prints the binary data of a bitmap in a chess board grid */
void wr_bitmap(const Bitmap map, ostream & output) {
    
    uint64_t mask = ( ((uint64_t) 1) << 63);
    for (int i = 0; i < 64; ++i) {
        
        if (map & mask)
            output << '1';
        else
            output << '0';
        mask >>= 1;

        if (i % 8 == 7)
            output << "\n";
    }
}

/* prints out 64 bits of binary data from MSB (left) to LSB (right) */
void wr_bin_64(uint64_t data, ostream & output) {
    
    uint64_t mask = ( ((uint64_t) 1) << 63);
    
    do {
        output << ((data & mask) ? '1' : '0');
    } while (mask >>= 1);
    
}

/*************************************************************************************
 PRINTING FUNCTIONS         for board, bitmap
 ************************************************************************************/

/* print out a human readable chess representation of the board */
void pr_board(const Board & b, const string & indent) {
    wr_board(b, indent, cout);
}

void pr_board(const Board & b) {
    wr_board(b, "", cout);
}


void pr_board_conf(const Board & b, const string & indent) {
    wr_board_conf(b, indent, cout);
}

void pr_board_conf(const Board & b) {
    wr_board_conf(b, "", cout);
}

/* prints the binary data of a bitmap in a chess board grid */
void pr_bitmap(const Bitmap map) {
    wr_bitmap(map, cout);
}

/* prints out 64 bits of binary data from MSB (left) to LSB (right) */
void pr_bin_64(uint64_t data) {
    wr_bin_64(data, cout);
}


