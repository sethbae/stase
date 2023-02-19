#include <iostream>
#include <ostream>
#include <string>
#include <vector>
using std::vector;
using std::string;
using std::ostream;
using std::cout;

#include "board.h"
#include "board.hpp"


/**
 * Converts the given SAN string (eg "Be4") into a Move, operating on the given board.
 * Requires disambiguations and checks to be used. If the move does not appear to describe
 * any legal move, then MOVE_SENTINEL is returned.
 */
Move stom(const Board & b, const string & san) {

    for (const Move m : game_rules::legal_moves(b)) {
        if (mtos(b, m) == san) {
            return m;
        }
    }

    if (san.size() == 4) {

        const Square from = stosq(san.substr(0, 2));
        const Square to = stosq(san.substr(2, 4));

        if (val(from) && val(to)) {
            return Move{from, to, 0};
        }
    }

    return MOVE_SENTINEL;
}

/**
 * An enum for the different strategies by which to disambiguate moves (e.g. Nbd2).
 */
enum DisambigType {
    NONE,
    BY_RANK,
    BY_FILE,
    TOTAL
};

/**
 * Checks whether disambiguation is needed for the given move on the given board. An appropriate
 * type (e.g. rank or file) is returned. If disambiguation is required, and either rank or file
 * would be effective, then file is returned as the default option.
 */
DisambigType compute_disambig(const Board & b, const Move m) {

    std::vector<Move> legals = game_rules::legal_moves(b);
    bool required = false;
    bool rank_not_usable = false;
    bool file_not_usable = false;

    for (const Move & other_m : legals) {

        if (equal(other_m.from, m.from)) { continue; }

        // moves of same type piece to the same destination
        if (equal(other_m.to, m.to)
                && type(b.get(other_m.from)) == type(b.get(m.from))) {

            required = true;

            // sharing start rank
            if (get_y(other_m.from) == get_y(m.from)) {
                rank_not_usable = true;
            }
            // sharing start file
            else if (get_x(other_m.from) == get_x(m.from)) {
                file_not_usable = true;
            }

        }
    }

    if (!required) {
        return NONE;
    }

    if (rank_not_usable && file_not_usable) {
        return TOTAL;
    } else if (file_not_usable) {
        return BY_RANK;
    } else {
        // using files is somewhat more natural, so it's the default
        return BY_FILE;
    }
}

/**
 * Converts a move to SAN. This depends on the board, which you must therefore provide.
 * The board must represent the position before the given move has been played.
 */
string mtos(const Board & b, const Move m) {

    if (is_sentinel(m)) {
        return "no move";
    }
    if (equal(m.from, empty_move().from) && equal(m.to, empty_move().to)) {
        return "empty move";
    }
    if (equal(m.from, m.to)) {
        return "invalid move";
    }

    string s;
    Colour piece_colour = colour(b.get(m.from));

    bool castle = (type(b.get(m.from)) == KING && abs(get_x(m.to) - get_x(m.from)) == 2);
    bool capture = (b.get(m.to) != EMPTY);
    bool prom = (type(b.get(m.from)) == PAWN && (get_y(m.to) == 7 || get_y(m.to) == 0));
    DisambigType disambig = compute_disambig(b, m);

    if (!castle) {
    
        // add character for piece type
        s += ptos_alg(b.get(m.from));

        switch (disambig) {
            case NONE: break;
            case BY_RANK: s += (char) ('1' + get_y(m.from)); break;
            case BY_FILE: s += (char) (get_x(m.from) + 'a'); break;
            case TOTAL: s += sqtos(m.from); break;
        }
        
        // for captures add x
        if (capture) {
            if (prom || type(b.get(m.from)) == PAWN) {
                if (disambig == NONE) {
                    s += (char) (get_x(m.from) + 'a');
                }
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
    Colour opposite_colour = (piece_colour == WHITE) ? BLACK : WHITE;
    if (game_rules::in_check(b, opposite_colour)) {
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

Board board_utils::fen_to_board(const std::string_view & fen) {

    std::string s(fen);
    stringstream stream(s);
    Board b = Board::empty();
    
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
                return Board::empty();
            }
            
        } else if ('1' <= c && c <= '8') {
            
            // legal integer: skip forward in row, printing empty
            x += (c - '0');

        } else {
            // illegal character
            return Board::empty();
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

string board_utils::board_to_fen(const Board & b) {
    
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

/*************************************************************************************
 WRITING FUNCTIONS         for board                write to output stream
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
                output << "\tFEN: " << board_utils::board_to_fen(b);
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

            default: break;
        }
        output << "\n";
    }
}

void board_utils::write_conf(const Board & b, ostream & o) {
    wr_board_conf(b, "", o);
}

/*************************************************************************************
 PRINTING FUNCTIONS         for board
 ************************************************************************************/

void board_utils::print(const Board & b) {
    wr_board(b, "", cout);
}

void board_utils::print_conf(const Board & b) {
    wr_board_conf(b, "", cout);
}
