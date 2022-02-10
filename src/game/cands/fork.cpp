#include "game.h"
#include "../gamestate.hpp"
#include "cands.h"
#include "responder.hpp"

/**
 * Returns true if the square contains a piece which is not favourably controlled
 * for its colour (ie non-negative for black, non-positive for white).
 */
inline bool zero_or_worse_control(Gamestate & gs, const Square s) {
    SquareControlStatus status =
        colour(gs.board.get(s)) == WHITE
            ? gs.control_cache->safe_get(gs, s)
            : gs.control_cache->safe_get(gs, s);
    if (colour(gs.board.get(s)) == WHITE) {
        return status.balance <= 0 && status.min_w > piece_value(PAWN);
    } else {
        return status.balance >= 0 && status.min_b > piece_value(PAWN);
    }
}

// TODO: interrupting poly x-rays

// TODO: defence along the line of the fork: 8/8/4Q3/8/8/1K3b2/8/8 b - - 0 1

// TODO: fork with capture?? 8/8/8/4b3/8/2P5/8/R3K3 w - - 0 1
//  "rnbqkbnr/pp2pppp/2pp4/P5B1/8/N2P4/1PP1PPPP/R2QKBNR b KQkq - 0 1"
//  "r3kbnr/p3pppp/n1p5/8/2P5/1P2PQ2/PB3PPP/RN3RK1 w - - 0 1"
//  "7r/8/8/2Q1b3/8/6p1/8/8 w - - 0 1"

// TODO: reject pawns defended by pawns! 4r2k/3q2p1/6Qp/p3p3/8/2P2R1P/6PK/8 w - - 2 37

//TODO: (other forks) well defended king can always be forked

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
        Square s2 = first_piece_encountered(gs.board, s, d);
        Piece p2 = gs.board.get(s2);

        if (is_sentinel(s2)) { continue; }
        if (p2 == EMPTY || colour(p) != colour(p2)) { continue; }
        if (abs(s.x - s2.x) <= 1 && abs(s.y - s2.y) <= 1) { continue; }

        // queens are the only piece you want to fork along a direction they can move:
        //  - bishops along diagonal: don't fork with another bishop or with a queen
        //  - rooks along straight: don't fork with rooks or queens, obviously.
        // TODO: check here for pins.
        if (type(p) != QUEEN && can_move_in_direction(p, d)) { continue; }
        if (type(p2) != QUEEN && can_move_in_direction(p2, d)) { continue; }

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
                       piece_value(KING) + 1,
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
 * If a square reaches 2, then it is added to the fork_squares vector.
 * If the parameter avoid is filled, then this delta will not be considered: use this for when
 * a piece is pinned. Neither direction of the avoid delta will be considered!
 */
inline void update_fork_net(
        Gamestate & gs,
        const Square s,
        uint8_t * fork_bools,
        Square * other_forked_piece,
        std::vector<FeatureFrame> & frames,
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
                    frames.push_back(
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
                    frames.push_back(
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
        const Gamestate & gs, const Square s, uint8_t * fork_bools, Square * other_forked_piece, std::vector<FeatureFrame> & frames) {

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
                    frames.push_back(
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
                    frames.push_back(
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

    std::vector<FeatureFrame> fork_frames;

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

    for (const FeatureFrame & ff : fork_frames) {

        // don't record traditional forks: only those which go in different directions
        Delta d1 = get_delta_between(ff.centre, itosq(ff.conf_1));
        Delta d2 = get_delta_between(ff.secondary, itosq(ff.conf_1));
        if (parallel(d1, d2)) { continue; }

        // don't record forks from unsafe squares
        SquareControlStatus status = gs.control_cache->safe_get(gs, itosq(ff.conf_1));
        if (colour(p) == WHITE
            && (status.balance < 0 || status.min_b < piece_value(KING))) { continue; }
        else if (colour(p) == BLACK
            && (status.balance > 0 || status.min_w < piece_value(KING))) { continue; }

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
            || would_be_unsafe_after(gs, fork_square, Move{s, fork_square, 0})) {
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
inline bool non_pawn_enemy(const Piece pawn, const Piece enemy) {
    // TODO: it is relevant here whether the piece is pinned or not, if it were pinned,
    //  it would be allowed to be a pawn.
    return enemy != EMPTY && colour(enemy) != colour(pawn) && type(enemy) != PAWN;
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

    // TODO: check the pins here - direction matters for captures.

    Piece pawn = gs.board.get(s);
    const int FORWARD = colour(pawn) == WHITE ? 1 : -1;
    const bool FIRST_MOVE = colour(pawn) == WHITE ? s.y == 1 : s.y == 6;

    if (type(pawn) != PAWN || gs.is_kpinned_piece(s, delta(0, FORWARD))) { return true; }

    std::vector<Square> squares;

    Square lcap = mksq(s.x - 1, s.y + FORWARD);
    Square rcap = mksq(s.x + 1, s.y + FORWARD);
    Square fw1 = mksq(s.x, s.y + FORWARD);
    Square fw2 = mksq(s.x, s.y + FORWARD + FORWARD);

    // regular captures
    if (val(lcap) && gs.board.get(lcap) != EMPTY && colour(gs.board.get(lcap)) != colour(pawn)) {
        squares.push_back(lcap);
    }
    if (val(rcap) && gs.board.get(rcap) != EMPTY && colour(gs.board.get(rcap)) != colour(pawn)) {
        squares.push_back(rcap);
    }
    // forward moves
    if (val(fw1) && gs.board.get(fw1) == EMPTY) {
        squares.push_back(fw1);
        if (FIRST_MOVE && gs.board.get(fw2) == EMPTY) {
            squares.push_back(fw2);
        }
    }
    // en-passant
    if (gs.board.get_ep_exists()) {
        // note that we do not risk duplicating, because ep squares are always empty
        if (equal(lcap, gs.board.get_ep_sq())) {
            squares.push_back(lcap);
        } else if (equal(rcap, gs.board.get_ep_sq())) {
            squares.push_back(rcap);
        }
    }

    for (int i = 0; i < squares.size(); ++i) {

        Square fork_square = squares[i];

        if (!would_be_unsafe_after(gs, fork_square, Move{s, fork_square})) {

            Square l_sq = mksq(fork_square.x - 1, fork_square.y + FORWARD);
            Square r_sq = mksq(fork_square.x + 1, fork_square.y + FORWARD);

            if (val(l_sq) && non_pawn_enemy(pawn, gs.board.get(l_sq))
                && val(r_sq) && non_pawn_enemy(pawn, gs.board.get(r_sq))) {
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
            Square sq1, sq2;

            for (int j = 0; j < 8; ++j) {

                Square forked_sq = mksq(fork_sq.x + XD[j], fork_sq.y + YD[j]);

                // TODO (GM-34): until beta-covers correctly handles knights, pawns and kings,
                //  this check is necessary. If the total distance from the king's starting
                //  square is less than 1 in all directions, the piece was already attacked.
                int xd = forked_sq.x - s.x, yd = forked_sq.y - s.y;
                if ((-1 <= xd && xd <= 1) && (-1 <= yd && yd <= 1)) { continue; }

                if (val(forked_sq)
                    && gs.board.get(forked_sq) != EMPTY
                    && zero_or_worse_control(gs, forked_sq)) {

                    if (count % 2 == 0) {
                        sq1 = forked_sq;
                    } else {
                        sq2 = forked_sq;
                    }
                    ++count;
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
void find_piece_to_fork(const Gamestate & gs, const FeatureFrame * ff, Move * m, IndexCounter & counter) {

    bool forker_is_white = colour(gs.board.get(ff->centre)) == BLACK;
    Delta d = get_delta_between(ff->centre, ff->secondary);

    // go over all pieces of the appropriate colour
    Square * sq_ptr = forker_is_white ? gs.wpieces : gs.bpieces;
    for (Square temp = *sq_ptr; !is_sentinel(temp); ++sq_ptr) {

        Piece p = gs.board.get(temp);
        if (!can_move_in_direction(p, d)) {
            continue;
        }

        // find any squares that the piece can move to on the required line
        Square endpoint = mksq(ff->secondary.x - d.dx, ff->secondary.y - d.dy);
        std::vector<Square> squares_reachable;
        if (type(p) == QUEEN) {
            // queens might be able to reach more than one square
            squares_reachable =
                squares_piece_can_reach_on_line(
                    gs.board,
                    temp,
                    ff->centre,
                    endpoint
                );
        } else {
            // other pieces can only reach a single square on a line
            Square reachable =
                square_piece_can_reach_on_line(
                    gs.board,
                    temp,
                    ff->centre,
                    endpoint
                );
            if (is_sentinel(reachable)) {
                squares_reachable = {};
            } else {
                squares_reachable = { reachable };
            }
        }

        // and check for each one that the piece is not pinned, and that it would end up safe.
        for (const Square fork_sq : squares_reachable) {
            if (gs.is_kpinned_piece(temp, get_delta_between(temp, fork_sq))
                || would_be_unsafe_after(gs, fork_sq, Move{temp, fork_sq})) {
                continue;
            }
            if (counter.has_space()) {
                Move move{temp, fork_sq, 0};
                move.set_score(fork_score(piece_value(gs.board.get(ff->centre)), piece_value(gs.board.get(ff->secondary))));
                m[counter.inc()] = move;
            } else {
                return;
            }
        }
    }
}

/**
 * This function responds to FeatureFrames of the second type, where the exact move has already been
 * found. It plays that move provided that it is to a safe square and does not violate any pins.
 */
void play_fork(const Gamestate & gs, const FeatureFrame * ff, Move * m, IndexCounter & counter) {

    Square forker_square = itosq(ff->conf_2);
    Square fork_square = itosq(ff->conf_1);

    if (!gs.is_kpinned_piece(forker_square, get_delta_between(forker_square, fork_square))
        && !would_be_unsafe_after(gs, fork_square, Move{forker_square, fork_square})) {

        if (counter.has_space()) {
            Move move{forker_square, fork_square, 0};
            move.set_score(fork_score(piece_value(gs.board.get(ff->centre)), piece_value(gs.board.get(ff->secondary))));
            m[counter.inc()] = move;
        }
    }
}

void seek_queen_to_play_fork(const Gamestate & gs, const FeatureFrame * ff, Move * m, IndexCounter & counter) {

    // find the queen which can fork
    Piece queen = colour(gs.board.get(ff->centre)) == WHITE ? B_QUEEN : W_QUEEN;
    Square q_sq = SQUARE_SENTINEL;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(x, y) == queen) {
                q_sq = mksq(x, y);
            }
        }
    }

    // check we have a queen
    if (is_sentinel(q_sq)) { return; }

    Square fork_sq = itosq(ff->conf_1);
    Delta d = get_delta_between(fork_sq, q_sq);

    // check we can move in the required direction
    if (!orth_diag(d) || gs.is_kpinned_piece(q_sq, d)) { return; }

    // establish that the run of squares to it is empty by using the first piece encountered
    Square fpe = first_piece_encountered(gs.board, q_sq, d);
    if (!is_sentinel(fpe)) {
        if (d.dx == 0) {
            // y is increasing, so we can reach any square on the line with y less than the fpe's, and vice versa
            if (d.dy > 0 && fpe.y < fork_sq.y) { return; }
            if (d.dy < 0 && fpe.y > fork_sq.y) { return; }
        } else {
            // x is increasing, so we can reach any square on the line with x less than the fpe's, and vice versa
            if (d.dx > 0 && fpe.x < fork_sq.x) { return; }
            if (d.dx < 0 && fpe.x > fork_sq.x) { return; }
        }
    }

    // safety checks are done on the hook side, so it's safe to play the fork!
    if (counter.has_space()) {
        Move move{q_sq, fork_sq, 0};
        m->set_score(
            fork_score(
                piece_value(gs.board.get(ff->centre)),
                piece_value(gs.board.get(ff->secondary))));
        m[counter.inc()] = move;
    }
}

void respond_to_fork_frame(const Gamestate & gs, const FeatureFrame * ff, Move * m, IndexCounter & counter) {
    if (ff->conf_1 == sq_sentinel_as_int()) {
        find_piece_to_fork(gs, ff, m, counter);
    } else if (type(gs.board.get(itosq(ff->conf_2))) == QUEEN) {
        seek_queen_to_play_fork(gs, ff, m, counter);
    } else {
        play_fork(gs, ff, m, counter);
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
bool capture_is_fork(Gamestate & gs, const Move m) {

    Piece p = gs.board.get(m.from);
    int forkable = 0;
    MoveType dir_array[2] = {ORTHO, DIAG};

    for (int i = 0; i < 2; ++i) {
        MoveType dir = dir_array[i];
        if (can_move_in_direction(p, dir)) {
            for (int j = 0; j < 4; ++j) {

                Delta d = D_ORTH[j];
                Square s = first_piece_encountered(gs.board, m.to, d);

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
