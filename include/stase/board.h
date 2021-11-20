#ifndef STASE_BOARD_H
#define STASE_BOARD_H

#include <string>
#include <vector>

#include "../../src/board/base_types.h"
#include "../../src/board/square.h"
#include "../../src/board/move.h"

/* query information about a given piece */
Ptype piece(Piece);
Ptype type(Piece);
Ptype colour(Piece);
bool is_white(Piece);
bool is_minor_piece(Piece);
bool is_major_piece(Piece);
bool is_not_pk(Piece);

/* a chess board! with full game state, such as castling rights, etc */
struct Board {

    mutable Byte squares[8][8];
    Int conf;

    inline Piece get(const int x, const int y) const {
        return squares[x][y];
    };
    inline void set(const int x, const int y, const Piece p) {
        squares[x][y] = p;
    }
    inline Piece get(const Square & s) const {
        return squares[s.x][s.y];
    };
    inline void set(const Square & s, const Piece p) {
        squares[s.x][s.y] = p;
    }

    Piece sneak(const Move) const;
    void unsneak(const Move, const Piece) const;

    void mutate(const Move);
    void mutate_hard(const Move); // does not assume move flags are correctly set

    Board successor(const Move) const;
    Board successor_hard(const Move) const;

    void set_conf_word(Int);
    Int get_conf_word() const;

    bool get_white() const;
    void set_white(bool);
    void flip_white();
    Ptype colour_to_move() const;

    bool get_cas_ws() const;
    void set_cas_ws(bool);
    bool get_cas_wl() const;
    void set_cas_wl(bool);
    bool get_cas_bs() const;
    void set_cas_bs(bool);
    bool get_cas_bl() const;
    void set_cas_bl(bool);

    bool get_ep_exists() const;
    void set_ep_exists(bool);
    Byte get_ep_file() const;
    void set_ep_file(Byte);
    Square get_ep_sq() const;

    unsigned get_halfmoves() const;
    void set_halfmoves(unsigned);
    void inc_halfmoves();
    unsigned get_wholemoves() const;
    void set_wholemoves(unsigned);
    void inc_wholemoves();

};

/* helper functions for board and move */
Move empty_move();
Board empty_board();
Board starting_pos();
std::string starting_fen();
Board fen_to_board(const std::string & fen);
std::string board_to_fen(const Board &);
std::vector<std::string> read_pgn(const std::string &s);


/* get moves for a piece or position */
Bitmap piecemoves(const Board &, const Square);
Bitmap piecemoves_ignore_check(const Board &, const Square);
void piecemoves(const Board &, const Square, std::vector<Move> &);
void piecemoves_ignore_check(const Board &, const Square, std::vector<Move> &);
bool in_check_hard(const Board &);
bool in_check_hard(const Board &, Ptype); // (colour specified)
bool in_check_attack_map(const Board &, Ptype);
void legal_piecemoves(const Board &, const Square, std::vector<Move> &);
Bitmap legal_piecemoves(const Board &, const Square);
void legal_moves(const Board &, std::vector<Move> &);
std::vector<Move> legal_moves(const Board &);

/* string functions for pieces, squares etc */
char ptoc(const Piece);
std::string ptos(const Piece);
std::string ptos_alg(const Piece);
Piece ctop(const char);

constexpr Square stosq(std::string_view str) {
    return mksq(str[0] - 'a', str[1] - '1');
}

std::string sqtos(const Square);
Move stom(const Board &, const std::string &);
std::string mtos(const Board &, const Move);

/* write functions (write to ostream) */
void wr_board(const Board &, std::ostream &);
void wr_board(const Board &, const std::string & indent, std::ostream &);
void wr_board_conf(const Board &, std::ostream &);
void wr_board_conf(const Board &, const std::string & indent, std::ostream &);
void wr_bitmap(const Bitmap, std::ostream &);
void wr_bin_64(uint64_t, std::ostream &);
/* print functions (write to stdout using cout) */
void pr_board(const Board &);
void pr_board(const Board &, const std::string & indent);
void pr_board_conf(const Board &);
void pr_board_conf(const Board &, const std::string & indent);
void pr_bitmap(const Bitmap);
void pr_bin_64(uint64_t);

/* Bitmap functions */
void set_square(Bitmap &, const Square);
void unset_square(Bitmap &, const Square);
bool test_square(const Bitmap &, const Square);

Bitmap knight_map(Square);
Bitmap king_map(Square);
Bitmap pattern_map(Square, Piece);
Bitmap vacancy_map(const Board &);
Bitmap occupancy_map(const Board &);
Bitmap friendly_map(const Board &);
Bitmap enemy_map(const Board &);
Bitmap attack_map(const Board &, Ptype);
Bitmap square_map(Square);
Square map_to_sq(Bitmap);
Bitmap custom_map(const Board &, bool include(const Board &, Square));
// Bitmap custom_map(const Board &, bool include(Square, Piece));

#endif //STASE_BOARD_H
