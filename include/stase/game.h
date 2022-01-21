#ifndef STASE_GAME_H
#define STASE_GAME_H

#include "board.h"
#include "../../src/game/eval.hpp"
#include "../../src/game/control_cache.hpp"

// and a lookup table (indexed by numeric value of Ptype enumeration) for which of
// the above movesets to use.
const bool PIECE_MOVE_TYPES[][6] = {
        { false, false, false, false, true, false },    // KING
        { true, true, false, false, false, false },     // QUEEN
        { true, false, false, false, false, false },    // ROOK
        { false, false, true, false, false, false },    // KNIGHT
        { false, true, false, false, false, false },    // BISHOP
        { false, false, false, true, false, false },    // PAWN
        { false, false, false, false, false, false },   // ---------invalid-----------
        { false, false, false, false, false, false },   // ---------invalid-----------
        { false, false, false, false, true, false },    // KING
        { true, true, false, false, false, false },     // QUEEN
        { true, false, false, false, false, false },    // ROOK
        { false, false, true, false, false, false },    // KNIGHT
        { false, true, false, false, false, false },    // BISHOP
        { false, false, false, true, false, false },    // PAWN
        { false, false, false, false, false, false }    // ---------invalid-----------
};

// returns true if the given piece can move in the given way, false otherwise
inline bool can_move_in_direction(Piece piece, MoveType dir) {
    return PIECE_MOVE_TYPES[piece][dir];
}
inline bool can_move_in_direction(Piece p, Delta d) {
    return can_move_in_direction(p, direction_of_delta(d));
}
bool can_move_to_square(const Board &, Square from, Square to);
bool collinear_points(Square, Square, Square);
Delta open_path_between(const Board & b, const Square, const Square);
Square square_piece_can_reach_on_line(const Board & b, const Square, const Square, const Square);
std::vector<Square> squares_piece_can_reach_on_line(const Board & b, const Square, const Square, const Square);
Square first_piece_encountered(const Board &, const Square, const Delta);

enum GamePhase : uint8_t {
    OPENING = 0,
    MIDGAME = 1,
    ENDGAME = 2
};
inline std::string name(GamePhase phase) {
    switch (phase) {
        case OPENING: return "OPENING";
        case MIDGAME: return "MIDGAME";
        case ENDGAME: return "ENDGAME";
        default: return "INVALID";
    }
}

// forward declaration: see cands.h
struct FeatureFrame;

enum CandList {
    CRITICAL,
    MEDIAL,
    FINAL,
    LEGAL
};
inline std::string name(CandList cand_list) {
    switch (cand_list) {
        case CRITICAL: return "CRITICAL";
        case MEDIAL: return "MEDIAL";
        case FINAL: return "FINAL";
        case LEGAL: return "LEGAL";
        default: return "INVALID";
    }
}

struct CandSet {
    std::vector<Move> critical;
    std::vector<Move> medial;
    std::vector<Move> final;
    std::vector<Move> legal;

    CandSet() :
        critical(std::vector<Move>()),
        medial(std::vector<Move>()),
        final(std::vector<Move>()),
        legal(std::vector<Move>())
        {}

    inline bool empty() { return critical.empty() && medial.empty() && final.empty() && legal.empty(); }
    inline int size() {
        if (legal.size()) {
            return legal.size();
        }
        return critical.size() + medial.size() + final.size();
    }
    inline std::vector<Move> get_list(CandList cand_list) {
        switch (cand_list) {
            case CRITICAL: return critical;
            case MEDIAL: return medial;
            case FINAL: return final;
            case LEGAL: return legal;
        }
        // impossible
        return {};
    }
    inline void clear_list(CandList cand_list) {
        switch (cand_list) {
            case CRITICAL: critical.clear(); break;
            case MEDIAL: medial.clear(); break;
            case FINAL: final.clear(); break;
            case LEGAL: legal.clear(); break;
        }
    }
};

struct Gamestate {

    // The enclosed board
    Board board;
    bool has_been_mated = false;
    bool w_cas = false;
    bool b_cas = false;
    bool in_check = false;
    Move last_move;
    Piece last_capture;
    mutable Square w_king;
    mutable Square b_king;
    GamePhase phase;

    FeatureFrame ** feature_frames;

    ControlCache * control_cache;

    explicit Gamestate();
    explicit Gamestate(const std::string &);
    explicit Gamestate(const std::string &, GamePhase);
    explicit Gamestate(const Board &);
    explicit Gamestate(const Gamestate &);
    explicit Gamestate(const Gamestate &, const Move);
    explicit Gamestate(Gamestate &&);
    ~Gamestate();

    Gamestate & operator=(const Gamestate &) = default;
    Gamestate & operator=(Gamestate &&) = default;

    void next_in_place(const Move);

    // TODO (GM-25): as and when required add these to the copy constructor

    // store a cache of squares on which pieces fall
    Square * wpieces;
    Square * bpieces;
    inline Square * friendly_pieces() const {
        return board.get_white() ? wpieces : bpieces;
    }
    inline Square * enemy_pieces() const {
        return board.get_white() ? bpieces : wpieces;
    }

    Square * w_kpinned_pieces;
    Delta * w_kpin_dirs;
    Square * b_kpinned_pieces;
    Delta * b_kpin_dirs;

    void add_kpinned_piece(const Square, const Delta);
    void remove_kpinned_piece(const Square);
    bool is_kpinned_piece(const Square, const Delta) const;

    inline Piece sneak(const Move m) const {
        Piece sneaked = board.sneak(m);
        Piece p = board.get(m.to);
        if (p == W_KING) {
            w_king = m.to;
        } else if (p == B_KING) {
            b_king = m.to;
        }
        return sneaked;
    }
    inline void unsneak(const Move m, const Piece sneaked) const {
        board.unsneak(m, sneaked);
        Piece p = board.get(m.from);
        if (p == W_KING) {
            w_king = m.from;
        } else if (p == B_KING) {
            b_king = m.from;
        }
    }

};

void print_cand_set(const Gamestate &, const CandSet &, std::ostream &);

// heuristic evaluation
Eval heur(const Gamestate &);
Eval heur_with_description(const Gamestate &);

// candidate moves
CandSet * cands(Gamestate &);
CandSet * cands(Gamestate &, CandSet *);
CandSet * cands_report(Gamestate &);

float piece_activity_alpha(const Board &);
float piece_activity_beta(const Board &);
float piece_activity_gamma(const Board &);

// control functions
int alpha_control(const Board &, Square);
int beta_control(const Board &, Square);
int gamma_control(const Board &, Square);

// cover functions
bool alpha_covers(const Board &, Square piece_sq, Square target_sq);
bool beta_covers(const Board &, Square piece_sq, Square target_sq);
bool gamma_covers(const Board &, Square piece_sq, Square target_sq);

bool is_safe_king(const Gamestate &, const Colour colour);
bool would_be_safe_king_square(const Gamestate &, const Square, const Colour colour);
bool would_be_safe_for_king_after(const Gamestate &, const Square, const Move, const Colour colour);

int control_count(const Board &, Square);
void display_control_counts(const Board &);

int w_pawn_defence_count(const Gamestate &, const Square);
int b_pawn_defence_count(const Gamestate &, const Square);
int pawn_defence_count(const Gamestate &, const Square);

SquareControlStatus evaluate_square_status(const Gamestate &, const Square);
bool is_weak_status(const Gamestate &, const Square, const Colour colour, SquareControlStatus);

// weak squares/unsafe pieces
bool is_weak_square(const Gamestate &, const Square, const Colour colour, const bool use_caches = true);
bool would_be_weak_after(const Gamestate &gs, const Square s, const Colour colour, const Move m);
bool is_unsafe_piece(const Gamestate &, const Square);
bool would_be_unsafe_after(const Gamestate &gs, const Square s, const Move m);

#endif //STASE_GAME_H
