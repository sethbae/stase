#ifndef STASE_GAMESTATE_HPP
#define STASE_GAMESTATE_HPP

#include "../../include/stase/game.h"
#include "../board/board.hpp"
#include "cands/cands.h"
#include "cands/hook.hpp"
#include "control_cache.hpp"
#include "pin_cache.hpp"

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
    PinCache wpin_cache;
    PinCache bpin_cache;
    PinCache wdiscoveries;
    PinCache bdiscoveries;

    ControlCache * control_cache;
    PieceEncounteredCache * pdir_cache;
    FeatureFrame frames[NUM_HOOKS][MAX_FRAMES];

    explicit Gamestate()
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

    explicit Gamestate(const Board & b)
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

    explicit Gamestate(const Gamestate & o, const Move m)
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

    explicit Gamestate(const std::string & fen)
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

    explicit Gamestate(const std::string & fen, GamePhase phase)
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

    explicit Gamestate(Gamestate && o)
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
        wpin_cache = o.wpin_cache;
        bpin_cache = o.bpin_cache;
        wdiscoveries = o.wdiscoveries;
        bdiscoveries = o.bdiscoveries;

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
    explicit Gamestate(const Gamestate & o)
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
        wpin_cache = o.wpin_cache;
        bpin_cache = o.bpin_cache;
        wdiscoveries = o.wdiscoveries;
        bdiscoveries = o.bdiscoveries;

        for (int i = 0; i < 16; ++i) {
            wpieces[i] = o.wpieces[i];
            if (is_sentinel(wpieces[i])) { break; }
        }
        for (int i = 0; i < 16; ++i) {
            bpieces[i] = o.bpieces[i];
            if (is_sentinel(bpieces[i])) { break; }
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
    void add_kpinned_piece(const Square s, const Delta delta) {
        Piece p = board.get(s);
        if (colour(p) == WHITE) {
            wpin_cache.add_pin(s, p, delta);
        } else {
            bpin_cache.add_pin(s, p, delta);
        }
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
    bool is_kpinned_piece(const Square s, const Delta delta) const {
        if (colour(board.get(s)) == WHITE) {
            return wpin_cache.is_pinned(s, delta);
        } else {
            return bpin_cache.is_pinned(s, delta);
        }
    }

    /**
     * If the piece on the given square is pinned to its king, this returns the delta along which it is pinned.
     * Otherwise, it returns INVALID_DELTA.
     */
    Delta delta_of_kpinned_piece(const Square s) const {
        if (colour(board.get(s)) == WHITE) {
            return wpin_cache.delta_of_pin(s);
        } else {
            return bpin_cache.delta_of_pin(s);
        }
    }

    /**
     * Checks if the piece has been pinned in any direction at all.
     */
    inline bool is_kpinned_piece(const Square s) {
        if (colour(board.get(s)) == WHITE) {
            return wpin_cache.is_pinned(s);
        } else {
            return bpin_cache.is_pinned(s);
        }
    }

    /**
     * Mark a piece as no longer pinned to its king, so that it is able to move once more.
     */
    void remove_kpinned_piece(const Square s) {
        if (colour(board.get(s)) == WHITE) {
            wpin_cache.remove_pin(s);
        } else {
            bpin_cache.remove_pin(s);
        }
    }

    /**
     * Records that the piece on the given square can move while giving a discovered check.
     * Add the square of the piece that moves, not the piece that gives check. Add the delta
     * along which the check would be given.
     */
    void add_discovery(const Square s, const Delta delta) {
        Piece p = board.get(s);
        if (colour(p) == WHITE) {
            wdiscoveries.add_pin(s, p, delta);
        } else {
            bdiscoveries.add_pin(s, p, delta);
        }
    }

    /**
     * Checks whether the given square contains a piece which can discover check.
     */
    bool can_discover_check(const Square s) const {
        // TODO (ST-118): un-hack this by generifying pin_cache.
        bool white = colour(board.get(s)) == WHITE;
        Delta d = get_delta_between(s, white ? w_king : b_king);
        if (white) {
            return wdiscoveries.is_pinned(s, d);
        } else {
            return bdiscoveries.is_pinned(s, d);
        }
    }

    /**
     * Mark a piece as no longer pinned to its king, so that it is able to move once more.
     */
    void remove_discovery(const Square s) {
        if (colour(board.get(s)) == WHITE) {
            wdiscoveries.remove_pin(s);
        } else {
            bdiscoveries.remove_pin(s);
        }
    }

private:
    /**
     * Allocates memory for all the fields in a gamestate which need it.
     */
    void alloc() {
        // initialise the piece lists with sentinels
        wpieces = new Square[16];
        bpieces = new Square[16];
        wpieces[0] = SQUARE_SENTINEL;
        bpieces[0] = SQUARE_SENTINEL;

        control_cache = new ControlCache;
        control_cache->gs = this;
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
