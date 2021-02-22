#include "board.h"

#include <iostream>
using std::cout;

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

/*************************************************************************************
 CONVERSIONS        for squares, FENs, SAN, pieces
 ************************************************************************************/

/***** square to string and string to square        (0, 0) <----> "a1"          *****/
Square stosq(string str) {
    return mksq(str[0] - 'a', str[1] - '1');
}

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

/***** Move to SAN and SAN to move (stubs) *****/

Move santomove(string san) {
    // needs to be able to find where the piece is coming from given its destination
    Move m = {0, 0, 0};
    return m;
}

string movetosan(Board & b, Move m) {
    // doesn't do captures, checks, promotions or disambiguations
    return ptos_alg(b.get(m.from)) + sqtos(m.to);
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

    // half and wholemoves
    b.set_halfmoves(get_number(stream));
    b.set_wholemoves(get_number(stream));
    
    return b;
}

string board_to_fen(const Board & b) {
    
    stringstream ss;

    // Build arrangement string
    for (int i = 7; i >= 0; --i) {
        int spaces = 0;
        for (int j = 0; j < 8; ++j) {
            char piece = ptoc(b.get(mksq(j, i)));
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

/***** useful functions for getting the start or empty boards without knowing the FENs *****/

Board starting_pos() {
    return fen_to_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board empty_board() {
    
    Board b;
    
    for (Square s = mksq(0, 0) ; val_y(s); inc_y(s)) {
        for ( ; val_x(s); inc_x(s)) {
            b.set(s, EMPTY);
        }
        reset_x(s);
    }
    
    return b;
}

/*************************************************************************************
 PRINTING FUNCTIONS         for board, bitmap
 ************************************************************************************/

/* print out a human readable chess representation of the board */
void pr_board(const Board & b, string indent) {
    for (int i = 7; i >= 0; --i) {
        cout << indent;
        for (int j = 0; j < 8; ++j) {
            Piece p = b.get(mksq(j, i));
            cout << ptoc(p) << " ";
        }
        cout << "\n";
    }
}

void pr_board(const Board & b) {
    pr_board(b, "");
}


void pr_board_conf(const Board & b, string indent) {
    for (int i = 7; i >= 0; --i) {
        cout << indent;
        for (int j = 0; j < 8; ++j) {
            Piece p = b.get(mksq(j, i));
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

void pr_board_conf(const Board & b) {
    pr_board_conf(b, "");
}

/* prints the binary data of a bitmap in a chess board grid */
// I changed this a bit but it's still wrong
void pr_bitmap(const Bitmap map) {
    
    uint64_t mask = ( ((uint64_t) 1) << 63);
    for (int i = 0; i < 64; ++i) {
        
        if (map & mask)
            cout << '1';
        else
            cout << '0';
        mask >>= 1;

        if (i % 8 == 7)
            cout << "\n";
    }
}

/* prints out 64 bits of binary data from MSB (left) to LSB (right) */
void pr_bin_64(uint64_t data) {
    
    uint64_t mask = ( ((uint64_t) 1) << 63);
    
    do {
        cout << ((data & mask) ? '1' : '0');
    } while (mask >>= 1);
    
}


