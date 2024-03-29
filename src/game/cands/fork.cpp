#include "../../../include/stase/game.h"
#include "../gamestate.hpp"
#include "cands.h"
#include "responder.hpp"

/**
 * Checks whether from this square, there are two pieces which can be forked.
 * This is done regardless of whether there is actually a piece threatening to
 * play this fork: the responder should deal with that side of things. The first
 * of the two pieces should be exactly on the given square. Does not duplicate
 * forks, in the sense the each fork will only be found from exactly one of
 * its two "endpoints".
 * Does not report pieces on adjacent squares, or forks with constraints which
 * can never be met (no sliding piece can fork a defended knight, for example).
 *
 * The format of the FeatureFrames is:
 * centre - the square of the first piece
 * secondary - the square of the second piece
 * conf1 - the value a piece must be worth strictly less than in order to play this fork
 * conf2 - unused (so SQUARE_SENTINEL)
 */
bool find_sliding_forks(Gamestate & gs, const Square s) {

    Piece p = gs.board.get(s);
    if (p == EMPTY) { return true; }

    /*
     * Find sliding forks first: check positive deltas only (negative will be found from the
     * other piece).
     */
    for (int i = 0; i < 4; ++i) {

        Delta d = D[i];
        Square s2 = gs.first_piece_encountered(s, d);
        if (is_sentinel(s2)) { continue; }

        Piece p2 = gs.board.get(s2);

        if (is_sentinel(s2)) { continue; }
        if (p2 == EMPTY || colour(p) != colour(p2)) { continue; }
        if (abs(s.x - s2.x) <= 1 && abs(s.y - s2.y) <= 1) { continue; }

        // queens are the only piece you want to fork along a direction they can move:
        //  - bishops along diagonal: don't fork with another bishop or with a queen
        //  - rooks along straight: don't fork with rooks or queens, obviously.
        if (type(p) != QUEEN && can_move_in_direction(p, d) && !gs.is_kpinned_piece(s, d)) { continue; }
        if (type(p2) != QUEEN && can_move_in_direction(p2, d) && !gs.is_kpinned_piece(s2, d)) { continue; }

        bool unconditional1 = zero_or_worse_control(gs, s);
        bool unconditional2 = zero_or_worse_control(gs, s2);

        if ((!unconditional1 && piece_value(p) < piece_value(ROOK))
            || (!unconditional2 && piece_value(p2) < piece_value(ROOK))) {
            continue;
        }

        if (unconditional1 && unconditional2) {
            if (!gs.add_frame(
                    fork_hook.id,
                    FeatureFrame{
                       s,
                       s2,
                       NOT_ATTACKED_AT_ALL,
                       sq_sentinel_as_int()
                    })) {
                return false;
            }
        } else {
            int val;
            if (unconditional1) {
                val = piece_value(p2);
            } else if (unconditional2) {
                val = piece_value(p);
            } else {
                val = min(piece_value(p), piece_value(p2));
            }
            // don't fork two defended pawns...
            if (val > piece_value(PAWN)) {
                if (!gs.add_frame(
                        fork_hook.id,
                        FeatureFrame{
                                s,
                                s2,
                                val,
                                sq_sentinel_as_int()
                        })) {
                    return false;
                }
            }
        }
    }

    return true;
}

/**
 * Works out from the given square along either ORTHO or DIAG deltas (according to parameter),
 * and records each square from which a forking piece could attack it in the fork_bools array.
 * If a square reaches 2, then it is added to fork_squares.
 * If the parameter avoid is filled, then this delta will not be considered: use this for when
 * a piece is pinned. Neither direction of the avoid delta will be considered!
 */
inline void update_fork_net(
        Gamestate & gs,
        const Square s,
        uint8_t * fork_bools,
        Square * other_forked_piece,
        ptr_vec<FeatureFrame> & frames,
        MoveType dir,
        const Delta avoid = INVALID_DELTA) {

    // retrieve the correct deltas to use
    const Delta * deltas;
    if (dir == ORTHO) {
        deltas = D_ORTH;
    } else {
        deltas = D_DIAG;
    }

    // work out from the piece along each delta, and mark up in the bools as appropriate
    for (int i = 0; i < 4; ++i) {

        Delta d = deltas[i];
        if (equal(d, avoid) || equal(delta(-d.dx, -d.dy), avoid)) { continue; }
        if (can_move_in_direction(gs.board.get(s), d)) { continue; }

        bool cont = true;
        int x = s.x + d.dx,
            y = s.y + d.dy;

        while (val(x, y) && cont) {

            Piece p = gs.board.get(x, y);

            if (p == EMPTY) {
                // empty: can be hit here, can continue looking
                if (fork_bools[8*x + y]++ == 1) {
                    frames.push(
                        FeatureFrame{
                            s,
                            other_forked_piece[8*x + y],
                            sqtoi(mksq(x, y)),
                            0  // these get filled in by the caller
                        });
                } else {
                    other_forked_piece[8*x + y] = s;
                }
            } else if (colour(p) == colour(gs.board.get(s))) {
                // own team: a capture could occur here and hit it, but stop looking
                if (fork_bools[8*x + y]++ == 1) {
                    frames.push(
                        FeatureFrame{
                            s,
                            other_forked_piece[8*x + y],
                            sqtoi(mksq(x, y)),
                            0  // these get filled in by the caller
                        });
                } else {
                    other_forked_piece[8*x + y] = s;
                }
                cont = false;
            } else {
                // other colour: no fork could occur here, and can't continue looking.
                cont = false;
            }
            x += d.dx;
            y += d.dy;
        }
    }
}

/**
 * This performs the same job as update_fork_net, but with some extra logic required to handle
 * pawns and their pins correctly.
 */
void update_pawn_fork_net(
        const Gamestate & gs, const Square s, uint8_t * fork_bools, Square * other_forked_piece, ptr_vec<FeatureFrame> & frames) {

    Delta avoid = gs.delta_of_kpinned_piece(s);
    int capturing_y_delta = (colour(gs.board.get(s)) == WHITE ? 1 : -1);

    for (int i = 0; i < 4; ++i) {

        Delta d = D_DIAG[i];
        bool cont = true;
        int x = s.x + d.dx,
            y = s.y + d.dy;

        if (d.dy == capturing_y_delta && !equal(d, delta(-avoid.dx, avoid.dy))) {
            // if we are working away from the pawn's "dangerous" side (ie, it could capture),
            // then in most cases we want to skip an extra square. If the pawn is pinned along
            // the opposite direction however, we also don't want to skip a square:
            //              . . k . .
            //              . . . p .
            //              . . Q . B
            // we can find that delta by flipping the x on the pawn's pin delta. If it wasn't pinned,
            // this won't matter anyway.
            x += d.dx;
            y += d.dy;
        }

        while (val(x, y) && cont) {

            Piece p = gs.board.get(x, y);

            if (p == EMPTY) {
                // empty: can be hit here, can continue looking
                if (fork_bools[8*x + y]++ == 1) {
                    frames.push(
                        FeatureFrame{
                            s,
                            other_forked_piece[8*x + y],
                            sqtoi(mksq(x, y)),
                            0  // these get filled in by the caller
                        });
                } else {
                    other_forked_piece[8*x + y] = s;
                }
            } else if (colour(p) == colour(gs.board.get(s))) {
                // own team: a capture could occur here and hit it, but stop looking
                if (fork_bools[8*x + y]++ == 1) {
                    frames.push(
                        FeatureFrame{
                            s,
                            other_forked_piece[8*x + y],
                            sqtoi(mksq(x, y)),
                            0  // these get filled in by the caller
                        });
                } else {
                    other_forked_piece[8*x + y] = s;
                }
                cont = false;
            } else {
                // other colour: no fork could occur here, and can't continue looking.
                cont = false;
            }
            x += d.dx;
            y += d.dy;
        }
    }
}

/**
 * Finds pieces which could be forked by a queen, regardless of whether there is actually a
 * legal (or safe) queen move to play the fork. These frames can be distinguished by conf_2,
 * which gives the current location of the putatively forking queen.
 *
 * The format of the FeatureFrames is this:
 * centre - the square of the first attacked piece
 * secondary - the square of the second attacked piece
 * conf_1 - the int-ised square on which the fork would be played
 * conf_2 - the int-ised current location of the queen
 */
bool find_queen_forks(Gamestate & gs, const Square s) {

    Piece p = gs.board.get(s);
    if (type(p) != QUEEN) { return true; }

    uint8_t fork_bools[64];
    for (int i = 0; i < 64; ++i) {
        fork_bools[i] = 0;
    }
    Square other_forked_piece[64];
    for (int i = 0; i < 64; ++i) {
        other_forked_piece[i] = SQUARE_SENTINEL;
    }

    FeatureFrame fork_frames_arr[64];
    ptr_vec<FeatureFrame> fork_frames(fork_frames_arr, 64);

    // first take all enemies not on a safe square
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Square forked_sq = mksq(x, y);
            Piece forked_p = gs.board.get(forked_sq);

            if (forked_p == EMPTY || colour(forked_p) == colour(p)) { continue; }

            if (type(forked_p) == QUEEN) { continue; }
            if (type(forked_p) == KING || zero_or_worse_control(gs, forked_sq)) {
                switch (type(forked_p)) {
                    case KING:
                        update_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames, ORTHO);
                        update_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames, DIAG);
                        break;
                    case ROOK:
                        update_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames, ORTHO,
                                        gs.delta_of_kpinned_piece(forked_sq));
                        update_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames, DIAG);
                        break;
                    case KNIGHT:
                        update_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames, ORTHO);
                        update_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames, DIAG);
                        break;
                    case BISHOP:
                        update_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames, ORTHO);
                        update_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames, DIAG,
                                        gs.delta_of_kpinned_piece(forked_sq));
                        break;
                    case PAWN:
                        update_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames, ORTHO);
                        update_pawn_fork_net(gs, forked_sq, fork_bools, other_forked_piece, fork_frames);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    for (int i = 0; i < fork_frames.size(); ++i) {

        FeatureFrame & ff = fork_frames[i];

        // don't record traditional forks: only those which go in different directions
        Square fork_sq = itosq(ff.conf_1);
        Delta d1 = get_delta_between(ff.centre, fork_sq);
        Delta d2 = get_delta_between(ff.secondary, fork_sq);
        if (parallel(d1, d2)) { continue; }

        // don't record forks from unsafe squares
        SquareControlStatus status = gs.control_cache->get_control_status(fork_sq);
        if (colour(p) == WHITE
            && (status.balance < 0 || status.min_b < piece_value(KING))) { continue; }
        else if (colour(p) == BLACK
            && (status.balance > 0 || status.min_w < piece_value(KING))) { continue; }

        // now check that the queen can actually play the fork
        Delta d = get_delta_between(s, fork_sq);

        // check we can move in the required direction
        if (!orth_diag(d) || gs.is_kpinned_piece(s, d)) { continue; }

        // establish that the run of squares to it is empty by using the first piece encountered
        Square fpe = gs.first_piece_encountered(s, d);

        if (!is_sentinel(fpe)) {
            if (d.dx == 0) {
                // y is increasing, so we can reach any square on the line with y less than the fpe's, and vice versa
                if (d.dy > 0 && fpe.y < fork_sq.y) { continue; }
                if (d.dy < 0 && fpe.y > fork_sq.y) { continue; }
            } else {
                // x is increasing, so we can reach any square on the line with x less than the fpe's, and vice versa
                if (d.dx > 0 && fpe.x < fork_sq.x) { continue; }
                if (d.dx < 0 && fpe.x > fork_sq.x) { continue; }
            }
        }

        if (!gs.add_frame(
                queen_fork_hook.id,
                FeatureFrame{
                    ff.centre,
                    ff.secondary,
                    ff.conf_1,
                    sqtoi(s)        // we put the queen here, regardless of whether the fork is safe or not.
                })) {
            return false;
        }
    }
    return true;
}

/**
 * Finds knights which can play forks. The knight is expected to be on the given square, but the
 * format of the FeatureFrame written corresponds to the frames written for other types of fork:
 * centre - the square of the first forkable piece
 * secondary - the square of the second forkable piece
 * conf1 - the square on which to play the fork
 * conf2 - the square on which the knight currently stands
 */
bool find_knight_forks(Gamestate & gs, const Square s) {

    if (type(gs.board.get(s)) != KNIGHT) { return true; }

    for (int j = 0; j < 8; ++j) {

        Square fork_square = mksq(s.x + XKN[j], s.y + YKN[j]);

        if (!val(fork_square)
            || colour(gs.board.get(fork_square)) == colour(gs.board.get(s))
            || !move_is_safe(gs, Move{s, fork_square, 0})) {
            continue;
        }

        int forked_count = 0;
        Square sq1, sq2;

        for (int k = 0; k < 8; ++k) {

            Square forked_square = mksq(fork_square.x + XKN[k], fork_square.y + YKN[k]);

            if (!val(forked_square)
                || equal(forked_square, s)) {
                continue;
            }

            Piece p = gs.board.get(forked_square);

            if (p != EMPTY
                && colour(p) != colour(gs.board.get(s))
                && type(p) != KNIGHT
                && (piece_value(p) > piece_value(KNIGHT)
                    || zero_or_worse_control(gs, forked_square))) {
                if (forked_count % 2 == 0) {
                    sq1 = forked_square;
                } else {
                    sq2 = forked_square;
                }
                ++forked_count;
            }
        }

        if (forked_count >= 2) {
            if (!gs.add_frame(
                    fork_hook.id,
                    FeatureFrame{
                        sq1,
                        sq2,
                        sqtoi(fork_square),
                        sqtoi(s)})) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Returns true if the second, enemy piece differs to colour to the first (pawn) piece,
 * and if the enemy is also not a pawn.
 */
inline bool forkable_by_pawn(const Gamestate & gs, const Square forking_sq, const Square forked_sq) {

    Piece pawn = gs.board.get(forking_sq);
    Piece enemy = gs.board.get(forked_sq);
    if (enemy == EMPTY) { return false; }
    if (colour(enemy) == colour(pawn)) { return false; }

    if (type(enemy) != PAWN) { return true; }
    return gs.is_kpinned_piece(forked_sq, get_delta_between(forking_sq, forked_sq));
}

/**
 * Finds pawns which can play forks. The pawn is expected to be on the given square, but the
 * format of the FeatureFrame written corresponds to the frames written for other types of fork:
 * centre - the square of the first forkable piece
 * secondary - the square of the second forkable piece
 * conf1 - the square on which to play the fork
 * conf2 - the square on which the pawn currently stands
 */
bool find_pawn_forks(Gamestate & gs, const Square s) {

    Piece pawn = gs.board.get(s);
    const int FORWARD = colour(pawn) == WHITE ? 1 : -1;
    const bool FIRST_MOVE = colour(pawn) == WHITE ? s.y == 1 : s.y == 6;

    if (type(pawn) != PAWN || gs.is_kpinned_piece(s, delta(0, FORWARD))) { return true; }

    Square squares_arr[4];
    ptr_vec<Square> squares(squares_arr, 4);

    Square lcap = mksq(s.x - 1, s.y + FORWARD);
    Square rcap = mksq(s.x + 1, s.y + FORWARD);
    Square fw1 = mksq(s.x, s.y + FORWARD);
    Square fw2 = mksq(s.x, s.y + FORWARD + FORWARD);

    // regular captures
    if (val(lcap) && gs.board.get(lcap) != EMPTY && colour(gs.board.get(lcap)) != colour(pawn)) {
        squares.push(lcap);
    }
    if (val(rcap) && gs.board.get(rcap) != EMPTY && colour(gs.board.get(rcap)) != colour(pawn)) {
        squares.push(rcap);
    }
    // forward moves
    if (val(fw1) && gs.board.get(fw1) == EMPTY) {
        squares.push(fw1);
        if (FIRST_MOVE && gs.board.get(fw2) == EMPTY) {
            squares.push(fw2);
        }
    }
    // en-passant
    if (gs.board.get_ep_exists()) {
        // note that we do not risk duplicating, because ep squares are always empty
        if (equal(lcap, gs.board.get_ep_sq())) {
            squares.push(lcap);
        } else if (equal(rcap, gs.board.get_ep_sq())) {
            squares.push(rcap);
        }
    }

    for (int i = 0; i < squares.size(); ++i) {

        Square fork_square = squares[i];

        if (move_is_safe(gs, Move{s, fork_square, 0})) {

            Square l_sq = mksq(fork_square.x - 1, fork_square.y + FORWARD);
            Square r_sq = mksq(fork_square.x + 1, fork_square.y + FORWARD);

            if (val(l_sq) && forkable_by_pawn(gs, s, l_sq)
                && val(r_sq) && forkable_by_pawn(gs, s, r_sq)) {
                if (!gs.add_frame(
                        fork_hook.id,
                        FeatureFrame{
                          l_sq,
                          r_sq,
                          sqtoi(fork_square),
                          sqtoi(s)
                        })) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Finds kings which can play forks. The king is expected to be on the given square, but the
 * format of the FeatureFrame written corresponds to the frames written for other types of fork:
 * centre - the square of the first forkable piece
 * secondary - the square of the second forkable piece
 * conf1 - the square on which to play the fork
 * conf2 - the square on which the king currently stands
 */
bool find_king_forks(Gamestate & gs, const Square s) {

    Piece king = gs.board.get(s);
    Colour king_col = colour(king);

    if (type(king) != KING) { return true; }

    for (int i = 0; i < 8; ++i) {

        Square fork_sq = mksq(s.x + XD[i], s.y + YD[i]);

        if (val(fork_sq)
            && colour(gs.board.get(fork_sq)) != king_col
            && would_be_safe_king_square(gs, fork_sq, king_col)) {

            int count = 0;
            Square sq1 = SQUARE_SENTINEL, sq2 = SQUARE_SENTINEL;

            for (int j = 0; j < 8; ++j) {

                Square forked_sq = mksq(fork_sq.x + XD[j], fork_sq.y + YD[j]);

                // TODO (ST-34): until beta-covers correctly handles knights, pawns and kings,
                //  this check is necessary. If the total distance from the king's starting
                //  square is less than 1 in all directions, the piece was already attacked.
                int xd = forked_sq.x - s.x, yd = forked_sq.y - s.y;
                if ((-1 <= xd && xd <= 1) && (-1 <= yd && yd <= 1)) { continue; }

                if (val(forked_sq)
                    && gs.board.get(forked_sq) != EMPTY
                    && colour(gs.board.get(forked_sq)) != king_col
                    && zero_or_worse_control(gs, forked_sq)) {

                    if (count++ == 0) {
                        sq1 = forked_sq;
                    } else {
                        sq2 = forked_sq;
                    }
                }

            }

            if (count >= 2) {
                if (!gs.add_frame(
                        fork_hook.id,
                        FeatureFrame{
                            sq1,
                            sq2,
                            sqtoi(fork_sq),
                            sqtoi(s)})) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Finds forks applicable to the given square. If the square is empty, no forks will be found.
 * If the square contains a queen, bishop or rook, then forks will be found playable against
 * the given piece. Otherwise, frames will be found playable by the given piece.
 * The FeatureFrames written in each case are equivalent, and are distinguished by the value of
 * conf2. If conf2 contains the int-ised version of SQUARE_SENTINEL, then no actual fork has been
 * found. Otherwise, conf1 gives the square on which a fork can been played, and conf2 gives the
 * current square of a piece which can play it.
 */
bool find_forks_hook(Gamestate & gs, const Square s) {

    Piece p = gs.board.get(s);

    if (!find_sliding_forks(gs, s)) {
        return false;
    }

    switch (type(p)) {
        case KNIGHT: return find_knight_forks(gs, s);
        case KING: return find_king_forks(gs, s);
        case PAWN: return find_pawn_forks(gs, s);
        default: return true;
    }
}

/**
 * This function responds to FeatureFrames of the first format; frames which give two pieces which
 * could be forked. It tries to move a piece onto the line between them and finds all such possible
 * moves.
 */
int find_piece_to_fork(const Gamestate & gs, const FeatureFrame * ff, Move * m, int idx, int end) {

    Colour forked_piece_colour = colour(gs.board.get(ff->centre));
    Delta d = get_delta_between(ff->centre, ff->secondary);

    // go over all pieces of the appropriate colour
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {

            Square temp = mksq(x, y);
            Piece p = gs.board.get(temp);
            if (p == EMPTY || colour(p) == forked_piece_colour) { continue; }
            if (!can_move_in_direction(p, d) || piece_value(p) >= ff->conf_1) {
                continue;
            }

            // find any squares that the piece can move to on the required line
            Square endpoint = mksq(ff->secondary.x - d.dx, ff->secondary.y - d.dy);
            Square reachable_arr[3];
            ptr_vec<Square> reachable(reachable_arr, 3);
            if (type(p) == QUEEN) {
                // queens might be able to reach more than one square
                squares_piece_can_reach_on_line(
                    gs.board,
                    reachable,
                    temp,
                    ff->centre,
                    endpoint
                );
            } else {
                // other pieces can only reach a single square on a line
                Square reachable_sq =
                    square_piece_can_reach_on_line(
                        gs.board,
                        temp,
                        ff->centre,
                        endpoint
                    );
                if (!is_sentinel(reachable_sq)) {
                    reachable.push(reachable_sq);
                }
            }

            // and check for each one that the piece is not pinned, and that it would end up safe.
            for (int i = 0; i < reachable.size(); ++i) {
                Square fork_sq = reachable[i];
                if (gs.is_kpinned_piece(temp, get_delta_between(temp, fork_sq))
                    || !move_is_safe(gs, Move{temp, fork_sq, 0})) {
                    continue;
                }
                if (idx < end) {
                    Move move{temp, fork_sq, 0};
                    move.set_score(fork_score(piece_value(gs.board.get(ff->centre)), piece_value(gs.board.get(ff->secondary))));
                    m[idx++] = move;
                } else {
                    return idx;
                }
            }

        }
    }
    return idx;
}

/**
 * This function responds to FeatureFrames of the second type, where the exact move has already been
 * found. It plays that move provided that it is to a safe square and does not violate any pins.
 */
int play_fork(const Gamestate & gs, const FeatureFrame * ff, Move * m, int idx, int end) {

    Square forker_square = itosq(ff->conf_2);
    Square fork_square = itosq(ff->conf_1);

    if (!gs.is_kpinned_piece(forker_square, get_delta_between(forker_square, fork_square))
        && move_is_safe(gs, Move{forker_square, fork_square, 0})) {

        if (idx < end) {
            Move move{forker_square, fork_square, 0};
            move.set_score(fork_score(piece_value(gs.board.get(ff->centre)), piece_value(gs.board.get(ff->secondary))));
            m[idx++] = move;
        }
    }
    return idx;
}

int respond_to_fork_frame(const Gamestate & gs, const FeatureFrame * ff, Move * m, int idx, int end) {
    if (ff->conf_2 == sq_sentinel_as_int()) {
        return find_piece_to_fork(gs, ff, m, idx, end);
    } else {
        return play_fork(gs, ff, m, idx, end);
    }
}

const Hook fork_hook{
    "fork",
    2,
    &find_forks_hook
};

const Hook queen_fork_hook{
    "queen-fork",
    7,
    &find_queen_forks
};

const Responder play_fork_resp{
    "fork",
    &respond_to_fork_frame
};

/**
 * Assesses whether the given move, assumed to be a capture, is also a fork.
 */
[[maybe_unused]] bool capture_is_fork(Gamestate & gs, const Move m) {

    /*
     * This function could be used to reprioritise captures which are also forks.
     * They should always be found by the unsafe_piece hook regardless.
     */

    Piece p = gs.board.get(m.from);
    int forkable = 0;
    MoveType dir_array[2] = {ORTHO, DIAG};

    for (int i = 0; i < 2; ++i) {
        MoveType dir = dir_array[i];
        if (can_move_in_direction(p, dir)) {
            for (int j = 0; j < 4; ++j) {

                Delta d = D_ORTH[j];
                Square s = gs.first_piece_encountered(m.to, d);

                if (is_sentinel(s) || equal(s, m.from)) { continue; }
                Piece forked_p = gs.board.get(s);

                // avoid pieces of the same colour
                if (colour(p) == colour(forked_p)) { continue; }

                // avoid pieces which can move in this direction (and aren't pinned)
                if (can_move_in_direction(forked_p, d) && !gs.is_kpinned_piece(s, d)) { continue; }

                // finally, accept pieces more valuable than the attacker, or which are on unsafe squares
                if (piece_value(forked_p) <= piece_value(p) && !zero_or_worse_control(gs, s)) { continue; }

                if (++forkable == 2) {
                    return true;
                }
            }
        }
    }
    return false;
}
