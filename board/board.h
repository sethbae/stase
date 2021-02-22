#ifndef BOARD_H
#define BOARD_H

#include <string>

typedef uint_fast8_t Byte;
typedef uint_fast32_t Int;
typedef uint_fast8_t Square;
typedef Byte Piece;
typedef uint64_t Bitmap;

/* enumeration of the types of pieces primarily, but also for other useful properties,
    such as WHITE or BLACK, and KING or QUEEN */
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
    QUEEN = 16, // explicit values to read promotion pieces easily (see Move::prom_piece())
    ROOK = 17,
    KNIGHT = 18,
    BISHOP = 19,
    PAWN,
    
    BLACK,
    WHITE,
    
    INVALID
};

/* query information about a given piece */
Ptype piece(Piece);
Ptype type(Piece);
Ptype colour(Piece);
bool is_white(Piece);

/* a chess board! with full game state, such as castling rights, etc */
struct Board {
    
    /* board and configuration word */
    Byte squares[8][4];
    Int conf;
    
    /* get/set squares */
    Piece get(const Square &) const;
    void set(const Square &, const Piece);

    /* get/set the entire config word */
    void set_conf_word(Int);
    Int get_conf_word() const;
    
    /* whose turn it is */
    bool get_white() const;
    void set_white(bool);
    void flip_white();
    
    /* castling rights */
    bool get_cas_ws() const;
    void set_cas_ws(bool);
    bool get_cas_wl() const;
    void set_cas_wl(bool);
    bool get_cas_bs() const;
    void set_cas_bs(bool);
    bool get_cas_bl() const;
    void set_cas_bl(bool);
    
    /* en-passant */
    bool get_ep_exists() const;
    void set_ep_exists(bool);
    unsigned get_ep_file() const;
    void set_ep_file(unsigned);
    Square get_ep_sq() const;
    
    /* half and whole moves */
    unsigned get_halfmoves() const;
    void set_halfmoves(unsigned);
    void inc_halfmoves();
    unsigned get_wholemoves() const;
    void set_wholemoves(unsigned);
    void inc_wholemoves();
    
};

/* A move structure, which stores some flags etc and get/set methods */
struct Move {
    
    Square from;
    Square to;
    uint_fast16_t flags;

    bool is_prom() const;
    void set_prom();
    bool is_cas() const;
    void set_cas();
    bool is_ep() const;
    void set_ep();
    bool is_cap() const;
    void set_cap();
    bool is_cas_short() const;
    void set_cas_short();
    
    int get_ep_file() const;
    
    Piece get_prom_piece() const;
    void set_prom_piece(unsigned);
    Piece get_cap_piece() const;
    void set_cap_piece(Piece p);


};

/* helper functions for board and move */
void make_move(Board &, Move);
Board fen_to_board(const std::string & fen);
std::string board_to_fen(const Board &);
Board empty_board();
Board starting_pos();

// other functions still in move

Square mksq(const int, const int);

/* helper functions for transforming a square */
void inc_x(Square &);
void dec_x(Square &);
void inc_y(Square &);
void dec_y(Square &);
void diag_ur(Square &);
void diag_ul(Square &);
void diag_dr(Square &);
void diag_dl(Square &);
void reset_x(Square &);
void reset_y(Square &);
/* helper functions for getting info about a square */
int get_y(const Square &);
int get_x(const Square &);
bool val_y(const Square &);
bool val_x(const Square &);
bool val(const Square &);

/* string functions for pieces, squares etc */
char ptoc(const Piece);
std::string ptos(const Piece);
std::string ptos_alg(const Piece);
Piece ctop(const char);
Square stosq(const std::string);
std::string sqtos(const Square);
Move santomove(std::string);
std::string movetosan(Board &, Move);

/* print functions (write to stdout using cout) */
void pr(const Board &);
void pr_config(const Board &);
void pr_board_config(const Board &);
void raw(const Board &);
void pr_mask(uint64_t mask);
void pr_64bit(uint64_t l);

/* Bitmap functions */
Bitmap square_map(Square);
Bitmap column_map(unsigned);
Bitmap row_map(unsigned);
Bitmap negdiag_map(Square);
Bitmap posdiag_map(Square);
Bitmap columns_map(unsigned from, unsigned to);
Bitmap rows_map(unsigned from, unsigned to);
Bitmap pattern_map(Square, Piece);
Bitmap vacancy_map(const Board &);
Bitmap custom_map(const Board &, bool include(Square, Piece));

// Experimental
Bitmap get_obstructed_move_map(const Board &, Square);

#endif
