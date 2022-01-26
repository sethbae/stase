#include "game.h"
#include "cands/cands.h"
#include "control_cache.hpp"

const int MAX_FRAMES = 20;

class Gamestate {

public:
    Board board;
    bool has_been_mated = false;
    bool w_cas = false;
    bool b_cas = false;
    bool in_check = false;
    GamePhase phase;

    Move last_move;
    Piece last_capture;
    mutable Square w_king;
    mutable Square b_king;
    Square * wpieces;
    Square * bpieces;
    Square * w_kpinned_pieces;
    Delta * w_kpin_dirs;
    Square * b_kpinned_pieces;
    Delta * b_kpin_dirs;

    ControlCache * control_cache;
    FeatureFrame frames[NUM_HOOKS][MAX_FRAMES];

    Gamestate()
            : w_king(SQUARE_SENTINEL),
              b_king(SQUARE_SENTINEL),
              phase(OPENING),
              w_cas(false),
              b_cas(false),
              in_check(false) {
        alloc();
    }

    Gamestate(const Board & b)
            : board(b),
              phase(OPENING),
              w_cas(false),
              b_cas(false),
              in_check(false) {
        alloc();
        find_kings();
    }

    Gamestate(const Gamestate & o, const Move m)
            : board(o.board.successor_hard(m)), last_move(m) {

        alloc();

        phase = o.phase;
        update_phase(m);

        // this may not be true: it gets set by cands()
        in_check = false;

        if (o.board.get(m.from) == W_KING) {
            w_king = m.to;
            b_king = o.b_king;
        } else if (board.get(m.from) == B_KING) {
            w_king = o.w_king;
            b_king = m.to;
        } else {
            w_king = o.w_king;
            b_king = o.b_king;
        }

        if (m.is_cas() && colour(board.get(m.from)) == WHITE) {
            w_cas = true;
            b_cas = o.b_cas;
        } else if (m.is_cas() && colour(board.get(m.from)) == BLACK) {
            w_cas = o.w_cas;
            b_cas = true;
        } else {
            w_cas = o.w_cas;
            b_cas = o.b_cas;
        }

    }

    Gamestate(const std::string & fen)
            : board(fen_to_board(fen)),
              phase(OPENING),
              w_cas(false),
              b_cas(false),
              in_check(false) {
        alloc();
        find_kings();
    }

    Gamestate(const std::string & fen, GamePhase phase)
            : board(fen_to_board(fen)),
              phase(phase),
              w_cas(false),
              b_cas(false),
              in_check(false) {
        alloc();
        find_kings();
    }

    Gamestate(Gamestate && o) {
        board = o.board;
        // copy contents of feature frames across
        for (int i = 0; i < ALL_HOOKS.size(); ++i) {
            for (int j = 0; j < MAX_FRAMES; ++j) {
                frames[i][j] = o.frames[i][j];
                if (is_sentinel(o.frames[i][j].centre)) { break; }
            }
        }
        control_cache = o.control_cache;
        wpieces = o.wpieces;
        bpieces = o.bpieces;
        w_kpinned_pieces = o.w_kpinned_pieces;
        w_kpin_dirs = o.w_kpin_dirs;
        b_kpinned_pieces = o.b_kpinned_pieces;
        b_kpin_dirs = o.b_kpin_dirs;
        w_cas = o.w_cas;
        b_cas = o.b_cas;
        w_king = o.w_king;
        b_king = o.b_king;
        in_check = o.in_check;
        phase = o.phase;
        o.control_cache = nullptr;
        o.wpieces = nullptr;
        o.bpieces = nullptr;
        o.w_kpinned_pieces = nullptr;
        o.w_kpin_dirs = nullptr;
        o.b_kpinned_pieces = nullptr;
        o.b_kpin_dirs = nullptr;
    }

    /**
     * Copy constructor. This is painfully slow and should be avoided wherever possible.
     * Currently, it's one use case is when caching the puzzles read from file: they need
     * to be copied when a second or later benchmark asks for them.
     */
    Gamestate(const Gamestate & o) {
        alloc();
        board = o.board;
        // copy contents of feature frames across
        for (int i = 0; i < ALL_HOOKS.size(); ++i) {
            for (int j = 0; j < MAX_FRAMES; ++j) {
                frames[i][j] = o.frames[i][j];
                if (is_sentinel(o.frames[i][j].centre)) { break; }
            }
        }
        w_cas = o.w_cas;
        b_cas = o.b_cas;
        in_check = o.in_check;
        w_king = o.w_king;
        b_king = o.b_king;
        last_move = o.last_move;
        last_capture = o.last_capture;
        phase = o.phase;
    }

    ~Gamestate() {
        delete control_cache;
        delete wpieces;
        delete bpieces;
        delete w_kpinned_pieces;
        delete w_kpin_dirs;
        delete b_kpinned_pieces;
        delete b_kpin_dirs;
    }

    Gamestate & operator=(const Gamestate &) = default;
    Gamestate & operator=(Gamestate &&) = default;

    inline Square * friendly_pieces() const {
        return board.get_white() ? wpieces : bpieces;
    }
    inline Square * enemy_pieces() const {
        return board.get_white() ? bpieces : wpieces;
    }
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

    /**
     * Attempts to add a frame for the given hook. If there is space, then true is returned.
     * Otherwise, false is returned and no changes are made.
     */
    inline bool add_frame(const int hook_id, const FeatureFrame frame) {
        for (int i = 0; i < MAX_FRAMES; ++i) {
            if (is_sentinel(frames[hook_id][i].centre)) {
                frames[hook_id][i] = frame;
                if (i < MAX_FRAMES - 1) {
                    frames[hook_id][i + 1] = FeatureFrame{SQUARE_SENTINEL, SQUARE_SENTINEL, 0, 0};
                }
                return true;
            }
        }
        return false;
    }

    /**
     * Updates the current gamestate such that it now represents the position resulting
     * from the given move being played.
     */
    inline void next_in_place(const Move m) {

        update_phase(m);
        if (board.get(m.from) == W_KING) {
            w_king = m.to;
            if (m.is_cas()) {
                w_cas = true;
            }
        } else if (board.get(m.from) == B_KING) {
            b_king = m.to;
            if (m.is_cas()) {
                b_cas = true;
            }
        }
        last_move = m;
        board = board.successor_hard(m);

    }

    /**
     * Records that the piece on the given square is pinned to its king and is therefore
     * not able to move.
     */
    void add_kpinned_piece(const Square s, const Delta dir) {

        Square * pieces;
        Delta * dirs;
        if (colour(board.get(s)) == WHITE) {
            pieces = w_kpinned_pieces;
            dirs = w_kpin_dirs;
        } else {
            pieces = b_kpinned_pieces;
            dirs = b_kpin_dirs;
        }

        // find the sentinel
        for (; !is_sentinel(*pieces); ++pieces, ++dirs)
            ;

        // add a piece and delta
        *pieces++ = s;
        *dirs = dir;
        *pieces = SQUARE_SENTINEL;

    }

    /**
     * Checks whether the given square contains a piece which is pinned to its king,
     * and is therefore not able to move. The delta given should be the direction it
     * would putatively be moving in, and a return value of *true* indicates that it
     * is indeed pinned in that direction, ie it cannot be moved. False indicates that
     * it is not pinned with regards to that direction, and can move.
     * If the piece on the square cannot move in the direction passed, true will be
     * returned regardless of the pin direction.
     */
    bool is_kpinned_piece(const Square s, const Delta d) const {

        Square * pieces;
        Delta * dirs;
        if (colour(board.get(s)) == WHITE) {
            pieces = w_kpinned_pieces;
            dirs = w_kpin_dirs;
        } else {
            pieces = b_kpinned_pieces;
            dirs = b_kpin_dirs;
        }

        for (; !is_sentinel(*pieces); ++pieces, ++dirs) {
            // check that the square matches
            if (equal(s, *pieces)) {
                // check it can move in the direction requested
                Piece p = board.get(s);
                MoveType move_type = direction_of_delta(d);
                if (can_move_in_direction(p, move_type) || (type(p) == PAWN && move_type == DIAG)) {
                    Delta dir1 = *dirs;
                    Delta dir2 = {(SignedByte) -dir1.dx, (SignedByte) -dir1.dy };
                    // and check that it is pinned in that direction or its diametric opposite
                    return !equal(d, dir1) && !equal(d, dir2);
                }
                return true;
            }
        }
        return false;

    }

    /**
     * Mark a piece as no longer pinned to its king, so that it is able to move once more.
     */
    void remove_kpinned_piece(const Square s) {

        Square * pieces;
        Delta * dirs;
        if (colour(board.get(s)) == WHITE) {
            pieces = w_kpinned_pieces;
            dirs = w_kpin_dirs;
        } else {
            pieces = b_kpinned_pieces;
            dirs = b_kpin_dirs;
        }

        // iterate up to the piece
        for ( ; !is_sentinel(*pieces) && !equal(*pieces, s); ++pieces, ++dirs)
            ;

        if (is_sentinel(*pieces)) {
            // piece was not found: return
            return;
        } else {
            // piece was found: move the remainder of the list left one
            pieces++;
            dirs++;
            for (; !is_sentinel(*pieces); ++pieces, ++dirs) {
                *(pieces - 1) = *pieces;
                *(dirs - 1) = *dirs;
            }
            *(pieces - 1) = SQUARE_SENTINEL;
        }
    }

private:
    /**
     * Allocates memory for all the fields in a gamestate which need it.
     */
    void alloc() {
        control_cache = new ControlCache;
        wpieces = static_cast<Square*> (operator new(sizeof(Square) * 16));
        bpieces = static_cast<Square*> (operator new(sizeof(Square) * 16));
        w_kpinned_pieces = static_cast<Square*> (operator new(sizeof(Square) * 16));
        w_kpin_dirs = static_cast<Delta*> (operator new(sizeof(Delta) * 16));
        b_kpinned_pieces = static_cast<Square*> (operator new(sizeof(Square) * 16));
        b_kpin_dirs = static_cast<Delta*> (operator new(sizeof(Delta) * 16));

        // set all pointers to null
        for (int i = 0; i < ALL_HOOKS.size(); ++i) {
            frames[i][0] = FeatureFrame{SQUARE_SENTINEL, SQUARE_SENTINEL, 0, 0};
        }

        // add sentinels to the pieces lists
        *wpieces = SQUARE_SENTINEL;
        *bpieces = SQUARE_SENTINEL;
        *w_kpinned_pieces = SQUARE_SENTINEL;
        *b_kpinned_pieces = SQUARE_SENTINEL;
    }

    /**
     * Transitions the Gamestate's phase according to certain conditions, based on the Gamestate and
     * the given move which creates the new gamestate. This move shouldn't have been played yet: this
     * needs to look at the board prior to the update step.
     */
    inline void update_phase(const Move m) {
        switch (phase) {
            case OPENING:
                if (board.get_wholemoves() >= 10) {
                    phase = MIDGAME;
                }
                break;
            case MIDGAME:
                if (type(board.get(m.from)) == QUEEN && type(board.get(m.to)) == QUEEN) {
                    phase = ENDGAME;
                }
                break;
            case ENDGAME:
                break;
        }
    }

    /**
     * Updates the given gamestate's w_king and b_king fields to refer to the correct
     * squares of the kings.
     */
    inline void find_kings() {
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                Piece p = board.get(mksq(x, y));
                if (p == W_KING) {
                    w_king = mksq(x, y);
                } else if (p == B_KING) {
                    b_king = mksq(x, y);
                }
            }
        }
    }

};
