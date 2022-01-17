#include <iostream>
#include "game.h"
#include "cands/cands.h"

/**
 * Allocates memory for all the fields in a gamestate which need it.
 */
void alloc(Gamestate * gs) {
    gs->feature_frames = new FeatureFrame*[ALL_HOOKS.size()];
    gs->wpieces = static_cast<Square*> (operator new(sizeof(Square) * 16));
    gs->bpieces = static_cast<Square*> (operator new(sizeof(Square) * 16));
    gs->w_kpinned_pieces = static_cast<Square*> (operator new(sizeof(Square) * 16));
    gs->w_kpin_dirs = static_cast<Delta*> (operator new(sizeof(Delta) * 16));
    gs->b_kpinned_pieces = static_cast<Square*> (operator new(sizeof(Square) * 16));
    gs->b_kpin_dirs = static_cast<Delta*> (operator new(sizeof(Delta) * 16));

    // set all pointers to null
    for (int i = 0; i < ALL_HOOKS.size(); ++i) {
        gs->feature_frames[i] = nullptr;
    }

    // add sentinels to the pieces lists
    *gs->wpieces = SQUARE_SENTINEL;
    *gs->bpieces = SQUARE_SENTINEL;
    *gs->w_kpinned_pieces = SQUARE_SENTINEL;
    *gs->b_kpinned_pieces = SQUARE_SENTINEL;
}

/**
 * Updates the given gamestate's w_king and b_king fields to refer to the correct
 * squares of the kings.
 */
void find_kings(Gamestate * gs) {
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Piece p = gs->board.get(mksq(x, y));
            if (p == W_KING) {
                gs->w_king = mksq(x, y);
            } else if (p == B_KING) {
                gs->b_king = mksq(x, y);
            }
        }
    }
}

/**
 * Transitions the Gamestate's phase according to certain conditions, based on the Gamestate and
 * the given move which creates the new gamestate. This move shouldn't have been played yet: this
 * needs to look at the board prior to the update step.
 */
void update_phase(Gamestate * gs, const Move m) {
    switch (gs->phase) {
        case OPENING:
            if (gs->board.get_wholemoves() >= 10) {
                gs->phase = MIDGAME;
            }
            break;
        case MIDGAME:
            if (type(gs->board.get(m.from)) == QUEEN && type(gs->board.get(m.to)) == QUEEN) {
                gs->phase = ENDGAME;
            }
            break;
        case ENDGAME:
            break;
    }
}

Gamestate::Gamestate()
    : w_king(SQUARE_SENTINEL),
      b_king(SQUARE_SENTINEL),
      phase(OPENING),
      w_cas(false),
      b_cas(false) {
    alloc(this);
}

Gamestate::Gamestate(const Board & b)
    : board(b),
      phase(OPENING),
      w_cas(false),
      b_cas(false) {
    alloc(this);
    find_kings(this);
}

Gamestate::Gamestate(const Gamestate & o, const Move m)
    : board(o.board.successor_hard(m)), last_move(m) {

    alloc(this);
    update_phase(this, m);

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

Gamestate::Gamestate(const std::string & fen)
    : board(fen_to_board(fen)),
      phase(OPENING),
      w_cas(false),
      b_cas(false) {
    alloc(this);
    find_kings(this);
}

Gamestate::Gamestate(const std::string & fen, GamePhase phase)
    : board(fen_to_board(fen)),
      phase(phase),
      w_cas(false),
      b_cas(false) {
    alloc(this);
    find_kings(this);
}

Gamestate::Gamestate(Gamestate && o) {
    this->board = o.board;
    this->feature_frames = o.feature_frames;
    this->wpieces = o.wpieces;
    this->bpieces = o.bpieces;
    this->w_kpinned_pieces = o.w_kpinned_pieces;
    this->w_kpin_dirs = o.w_kpin_dirs;
    this->b_kpinned_pieces = o.b_kpinned_pieces;
    this->b_kpin_dirs = o.b_kpin_dirs;
    this->w_cas = o.w_cas;
    this->b_cas = o.b_cas;
    this->w_king = o.w_king;
    this->b_king = o.b_king;
    this->phase = o.phase;
    o.feature_frames = nullptr;
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
Gamestate::Gamestate(const Gamestate & o) {
    alloc(this);
    this->board = o.board;
    // copy contents of feature frame across
    for (int i = 0; i < ALL_HOOKS.size(); ++i) {
        if (o.feature_frames[i]) {

            FeatureFrame frames[64];

            int j = 0;
            while (!is_sentinel(o.feature_frames[i][j].centre)) {
                frames[j] = o.feature_frames[i][j];
                ++j;
            }

            this->feature_frames[i] = static_cast<FeatureFrame*> (operator new((sizeof(FeatureFrame)) * (j + 1)));
            for (int k = 0; k < j; ++k) {
                this->feature_frames[i][k] = o.feature_frames[i][k];
            }
            *this->feature_frames[j] = FeatureFrame{SQUARE_SENTINEL, 0, 0, 0};
        }
    }
    w_cas = o.w_cas;
    b_cas = o.b_cas;
    w_king = o.w_king;
    b_king = o.b_king;
    last_move = o.last_move;
    last_capture = o.last_capture;
    phase = o.phase;
}

Gamestate::~Gamestate() {
    for (int i = 0; i < ALL_HOOKS.size(); ++i) {
        delete feature_frames[i];
    }
    delete[] feature_frames;
    delete wpieces;
    delete bpieces;
    delete w_kpinned_pieces;
    delete w_kpin_dirs;
    delete b_kpinned_pieces;
    delete b_kpin_dirs;
}

/**
 * Populate all the information in the caches completely from scratch.
 */
void Gamestate::repopulate_caches() {
    // for each square, pieces
}

/**
 * Updates the current gamestate such that it now represents the position resulting
 * from the given move being played.
 */
void Gamestate::next_in_place(const Move m) {

    update_phase(this, m);
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
void Gamestate::add_kpinned_piece(const Square s, const Delta dir) {

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
bool Gamestate::is_kpinned_piece(const Square s, const Delta d) const {

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
void Gamestate::remove_kpinned_piece(const Square s) {

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
