#ifndef STASE_GAMESTATE_HPP
#define STASE_GAMESTATE_HPP

#include "../../include/stase/game.h"
#include "../board/board.hpp"
#include "cands/cands.h"
#include "cands/hook.hpp"
#include "control_cache.hpp"

const int MAX_FRAMES = 20;

class Gamestate {

public:
    Board board;
    bool game_over;
    bool w_cas;
    bool b_cas;
    bool in_check;
    GamePhase phase;

    Move last_move;
    mutable Square w_king;
    mutable Square b_king;
    Square * wpieces;
    Square * bpieces;
    Square * w_kpinned_pieces;
    Delta * w_kpin_dirs;
    Square * b_kpinned_pieces;
    Delta * b_kpin_dirs;

    ControlCache * control_cache;
    PieceEncounteredCache * pdir_cache;
    FeatureFrame frames[NUM_HOOKS][MAX_FRAMES];

    Gamestate()
            : game_over(false),
              w_cas(false),
              b_cas(false),
              in_check(false),
              phase(OPENING),
              w_king(SQUARE_SENTINEL),
              b_king(SQUARE_SENTINEL)
    {
        alloc();
    }

    Gamestate(const Board & b)
            : board(b),
              game_over(false),
              w_cas(false),
              b_cas(false),
              in_check(false),
              phase(OPENING)
    {
        alloc();
        find_kings();
        compute_cache(board, pdir_cache);
    }

    Gamestate(const Gamestate & o, const Move m)
            : board(o.board.successor_hard(m)),
              game_over(false),
              in_check(false),
              phase(o.phase),
              last_move(m)
    {
        alloc();

        if (o.board.get(m.from) == W_KING) {
            w_king = m.to;
            b_king = o.b_king;
        } else if (o.board.get(m.from) == B_KING) {
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

        // copy pdir_cache contents across and then update it
        copy_cache(o.pdir_cache, pdir_cache);
        update_cache(board, pdir_cache, m);
        update_phase(m);
        this->control_cache->update(o.board, *o.control_cache, m);
    }

    Gamestate(const std::string & fen)
            : board(fen_to_board(fen)),
              game_over(false),
              w_cas(false),
              b_cas(false),
              in_check(false),
              phase(OPENING)
    {
        alloc();
        find_kings();
        compute_cache(board, pdir_cache);
    }

    Gamestate(const std::string & fen, GamePhase phase)
            : board(fen_to_board(fen)),
              game_over(false),
              w_cas(false),
              b_cas(false),
              in_check(false),
              phase(phase)
    {
        alloc();
        find_kings();
        compute_cache(board, pdir_cache);
    }

    Gamestate(Gamestate && o)
    {
        board = o.board;
        game_over = o.game_over;
        w_cas = o.w_cas;
        b_cas = o.b_cas;
        in_check = o.in_check;
        phase = o.phase;

        last_move = o.last_move;
        w_king = o.w_king;
        b_king = o.b_king;
        wpieces = o.wpieces; o.wpieces = nullptr;
        bpieces = o.bpieces; o.bpieces = nullptr;
        w_kpinned_pieces = o.w_kpinned_pieces; o.w_kpinned_pieces = nullptr;
        w_kpin_dirs = o.w_kpin_dirs; o.w_kpin_dirs = nullptr;
        b_kpinned_pieces = o.b_kpinned_pieces;  o.b_kpinned_pieces = nullptr;
        b_kpin_dirs = o.b_kpin_dirs; o.b_kpin_dirs = nullptr;

        control_cache = o.control_cache;
        control_cache->gs = this;
        o.control_cache = nullptr;
        pdir_cache = o.pdir_cache; o.pdir_cache = nullptr;

        // copy contents of feature frames across
        for (int i = 0; i < ALL_HOOKS.size(); ++i) {
            for (int j = 0; j < MAX_FRAMES; ++j) {
                frames[i][j] = o.frames[i][j];
                if (is_sentinel(o.frames[i][j].centre)) { break; }
            }
        }
    }

    /**
     * Copy constructor. This is painfully slow and should be avoided wherever possible.
     * Currently, it's one use case is when caching the puzzles read from file: they need
     * to be copied when a second or later benchmark asks for them.
     */
    Gamestate(const Gamestate & o)
    {
        alloc();
        board = o.board;
        game_over = o.game_over;
        w_cas = o.w_cas;
        b_cas = o.b_cas;
        in_check = o.in_check;
        phase = o.phase;

        last_move = o.last_move;
        w_king = o.w_king;
        b_king = o.b_king;
        for (int i = 0; i < 16; ++i) {
            wpieces[i] = o.wpieces[i];
            if (is_sentinel(wpieces[i])) { break; }
        }
        for (int i = 0; i < 16; ++i) {
            bpieces[i] = o.bpieces[i];
            if (is_sentinel(bpieces[i])) { break; }
        }
        for (int i = 0; i < 16; ++i) {
            w_kpinned_pieces[i] = o.w_kpinned_pieces[i];
            if (is_sentinel(w_kpinned_pieces[i])) { break; }
        }
        for (int i = 0; i < 16; ++i) {
            w_kpin_dirs[i] = o.w_kpin_dirs[i];
            if (!is_valid_delta(w_kpin_dirs[i])) { break; }
        }
        for (int i = 0; i < 16; ++i) {
            b_kpinned_pieces[i] = o.b_kpinned_pieces[i];
            if (is_sentinel(b_kpinned_pieces[i])) { break; }
        }
        for (int i = 0; i < 16; ++i) {
            b_kpin_dirs[i] = o.b_kpin_dirs[i];
            if (!is_valid_delta(b_kpin_dirs[i])) { break; }
        }

        // copy the caches
        control_cache->copy(*o.control_cache);
        copy_cache(o.pdir_cache, pdir_cache);

        // copy contents of feature frames across
        for (int i = 0; i < ALL_HOOKS.size(); ++i) {
            for (int j = 0; j < MAX_FRAMES; ++j) {
                frames[i][j] = o.frames[i][j];
                if (is_sentinel(o.frames[i][j].centre)) { break; }
            }
        }
    }

    ~Gamestate() {
        delete control_cache;
        delete pdir_cache;
        delete[] wpieces;
        delete[] bpieces;
        delete[] w_kpinned_pieces;
        delete[] w_kpin_dirs;
        delete[] b_kpinned_pieces;
        delete[] b_kpin_dirs;
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

    inline void clear_frames(const int hook_id) {
        frames[hook_id][0] = FeatureFrame{ SQUARE_SENTINEL, SQUARE_SENTINEL, 0, 0 };
    }
    inline void clear_all_frames() {
        for (int i = 0; i < NUM_HOOKS; ++i) {
            frames[i][0] = FeatureFrame{ SQUARE_SENTINEL, SQUARE_SENTINEL, 0, 0 };
        }
    }

    /**
     * Walks out from the given square in the given direction until it reaches a piece.
     * It returns the square on which that piece lies, or SQUARE_SENTINEL if no piece was
     * encountered before the edge of the board.
     */
    inline Square first_piece_encountered(const Square s, const Delta d) const {
        return pdir_cache->d[(d.dx + 1)][(d.dy + 1)].p[s.x][s.y];
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
     * If the piece on the given square is pinned to its king, this returns the delta along which it is pinned.
     * Otherwise, it returns INVALID_DELTA.
     */
    Delta delta_of_kpinned_piece(const Square s) const {

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
                return *dirs;
            }
        }
        return INVALID_DELTA;
    }

    /**
     * Checks if the piece has been pinned in any direction at all.
     */
    inline bool is_kpinned_piece(const Square s) {
        return is_valid_delta(delta_of_kpinned_piece(s));
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
        wpieces = new Square[16];
        bpieces = new Square[16];
        w_kpinned_pieces = new Square[16];
        w_kpin_dirs = new Delta[16];
        b_kpinned_pieces = new Square[16];
        b_kpin_dirs = new Delta[16];

        // add sentinels to the pieces lists
        wpieces[0] = SQUARE_SENTINEL;
        bpieces[0] = SQUARE_SENTINEL;
        w_kpinned_pieces[0] = SQUARE_SENTINEL;
        w_kpin_dirs[0] = INVALID_DELTA;
        b_kpinned_pieces[0] = SQUARE_SENTINEL;
        b_kpin_dirs[0] = INVALID_DELTA;

        control_cache = new ControlCache; control_cache->gs = this;
        pdir_cache = new PieceEncounteredCache;

        // add sentinel to start of every list
        for (int i = 0; i < ALL_HOOKS.size(); ++i) {
            frames[i][0] = FeatureFrame{SQUARE_SENTINEL, SQUARE_SENTINEL, 0, 0};
        }
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

#endif  // STASE_GAMESTATE_HPP
