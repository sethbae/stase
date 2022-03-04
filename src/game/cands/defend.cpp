#include "board.h"
#include "cands.h"
#include "../gamestate.hpp"
#include "responder.hpp"

/**
 * Checks whether a piece of the given type, standing on the from square, could move
 * onto the to square. This allows for captures, and does not consider whether the piece
 * is pinned or not. This assumes that the piece is able to move in the direction leading
 * from from to to.
 */
bool can_see_immediately(const Gamestate & gs, const Piece p, const Square from, const Square to) {

    Delta d = get_delta_between(from, to);
    Square fpe = gs.first_piece_encountered(from, d);

    if (is_sentinel(fpe)) { return true; }
    if (d.dx == 0) {
        // y is increasing, so we can reach any square on the line with y less than the fpe's, and vice versa
        if (d.dy > 0 && to.y < fpe.y) { return true; }
        if (d.dy < 0 && to.y > fpe.y) { return true; }
        if (to.y == fpe.y) { return colour(gs.board.get(fpe)) != colour(p); }
    } else {
        // x is increasing, so we can reach any square on the line with x less than the fpe's, and vice versa
        if (d.dx > 0 && to.x < fpe.x) { return true; }
        if (d.dx < 0 && to.x > fpe.x) { return true; }
        if (to.x == fpe.x) { return colour(gs.board.get(fpe)) != colour(p); }
    }
    return false;
}

/**
 * Checks whether a piece of the given type, if it were standing on the from square,
 * would be able to control the to square, permitting x-rays. This does not take
 * account of pins, and assumes the piece given can indeed move in the direction required.
 */
bool can_see_x_ray(const Gamestate & gs, const Piece p, const Square from, const Square to) {

    Delta d = get_delta_between(from, to);
    bool x_changing = (d.dx != 0);
    bool already_x_raying = false;
    Square fpe = gs.first_piece_encountered(from, d);

    while (!is_sentinel(fpe) && ((x_changing && fpe.x < to.x) || (!x_changing && fpe.y < to.y))) {
        Piece other_p =  gs.board.get(fpe);
        if (colour(other_p) != colour(p)) {
            if (already_x_raying) {
                // if we hit an enemy piece and we were already in an x-ray, we can't see the square
                return false;
            } else {
                // otherwise, however, we can continue (now in an x-ray)
                already_x_raying = true;
                fpe = gs.first_piece_encountered(fpe, d);
                continue;
            }
        }
        if (can_move_in_direction(other_p, d)) {
            // x-rayable piece: loop
            fpe = gs.first_piece_encountered(fpe, d);
        } else {
            // not x-rayable, cannot see square
            return false;
        }
    }

    // sentinel encountered or square beyond(at) the target square
    return true;
}

/**
 * Finds squares on which a rook on p_sq would be able to defend c_sq. Does not account for whether
 * the rook is pinned or not. Does not return squares if the rook already covers the c_sq.
 */
void find_rook_cover(
        const Gamestate & gs, std::vector<Square> & squares, const Square p_sq, const Square c_sq) {

    if (p_sq.x == c_sq.x || p_sq.y == c_sq.y) {
        // piece already defends
        return;
    }

    Square top_left = mksq(p_sq.x, c_sq.y);
    Square top_right = mksq(c_sq.x, p_sq.y);

    if (can_see_immediately(gs, gs.board.get(p_sq), p_sq, top_left)
            && can_see_x_ray(gs, gs.board.get(p_sq), top_left, c_sq)) {
        squares.push_back(top_left);
    }
    if (can_see_immediately(gs, gs.board.get(p_sq), p_sq, top_right)
            && can_see_x_ray(gs, gs.board.get(p_sq), top_right, c_sq)) {
        squares.push_back(top_right);
    }
    return;
}

/**
 * Finds squares on which a bishop on p_sq would be able to defend c_sq. Does not account for whether
 * the rook is pinned or not. Does not return squares if the rook already covers the c_sq.
 */
void find_bishop_cover(const Gamestate & gs, std::vector<Square> & squares, const Square p_sq, const Square c_sq) {

    if (abs(c_sq.x - p_sq.x) == abs(c_sq.y - p_sq.y)) {
        // piece already defends
        return;
    }

    Square corner1, corner2;
    bool is_light_square = light_square(p_sq);
    if (light_square(c_sq) != is_light_square) { return; }
    int diag_diff = diag_ordinal(p_sq) - diag_ordinal(c_sq);

    if ((is_light_square && diag_diff > 0)
        || (!is_light_square && diag_diff < 0)) {
        // go up left, then down right
        corner1 = mksq(p_sq.x - abs(diag_diff), p_sq.y + abs(diag_diff));
        corner2 = mksq(c_sq.x + abs(diag_diff), c_sq.y - abs(diag_diff));
    } else {
        // go down right, then up left
        corner1 = mksq(p_sq.x + abs(diag_diff), p_sq.y - abs(diag_diff));
        corner2 = mksq(c_sq.x - abs(diag_diff), c_sq.y + abs(diag_diff));
    }

    if (val(corner1)
            && can_see_immediately(gs, gs.board.get(p_sq), p_sq, corner1)
            && can_see_x_ray(gs, gs.board.get(p_sq), corner1, c_sq)) {
        squares.push_back(corner1);
    }
    if (val(corner2)
            && can_see_immediately(gs, gs.board.get(p_sq), p_sq, corner2)
            && can_see_x_ray(gs, gs.board.get(p_sq), corner2, c_sq)) {
        squares.push_back(corner2);
    }
}

/**
 * Finds squares on which a bishop on p_sq would be able to defend c_sq. Does not account for whether
 * the rook is pinned or not. Does not return squares if the rook already covers the c_sq.
 */
void find_queen_cover(const Gamestate & gs, std::vector<Square> & squares, const Square q_sq, const Square c_sq) {

    if (beta_covers(gs.board, q_sq, c_sq)) {
        // queen already defends
        return;
    }

    // strategy is to find the segment of emtpy squares leading from c_sq to either a piece or the edge of the board
    for (int i = 0; i < 8; ++i) {

        Delta d = D[i];

        // TODO make this account for x-rays (use a while loop)
        Square segment_end = gs.first_piece_encountered(c_sq, d);
        if (is_sentinel(segment_end)) {
            segment_end = edge_of_board(c_sq, i);
        }
        if (equal(c_sq, segment_end)) { continue; }

        // try to get on the line c_sq (exclusive) -> segment_end (inclusive)
        // parameter ordering in following call is crucial (to get inc/exc right)
        std::vector<Square> reachable = squares_piece_can_reach_on_line(gs.board, q_sq, segment_end, c_sq);
        for (const Square & s : reachable) {
            squares.push_back(s);
        }
    }
}

/**
 * Given a list in which to accumulate possibilities, this finds squares that a sliding piece could
 * move to in order to defend the given cover square (c_sq). This does not check for pins or for
 * square safety: it just finds the moves.
 * This does nothing if given a non sliding piece (king, pawn or knight).
 */
void find_cover_squares(
        const Gamestate & gs, std::vector<Square> & squares, const Square piece_square, const Square cover_square) {
    switch (type(gs.board.get(piece_square))) {
        case BISHOP: find_bishop_cover(gs, squares, piece_square, cover_square); return;
        case ROOK: find_rook_cover(gs, squares, piece_square, cover_square); return;
        case QUEEN: find_queen_cover(gs, squares, piece_square, cover_square); return;
        default: return;
    }
}

void defend_square(const Gamestate & gs, const Square s, Move * moves, IndexCounter & counter) {

    const Colour defending_colour = gs.board.get_white() ? WHITE : BLACK;

    Square piece_squares[16];
    int pieces_point = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Square temp = mksq(x, y);
            if (colour(gs.board.get(temp)) == defending_colour
                && !equal(temp, s)) {
                piece_squares[pieces_point++] = temp;
            }
        }
    }

    for (int i = 0; i < pieces_point; ++i) {

        Square p_sq = piece_squares[i];
        switch (type(gs.board.get(p_sq))) {
            case KNIGHT:
            case KING:
            case PAWN:
                continue;
            default: ;
        }

        std::vector<Square> cover_squares;
        find_cover_squares(gs, cover_squares, p_sq, s);

        for (const Square & c_sq : cover_squares) {

            if (move_is_safe(gs, Move{p_sq, c_sq, 0})
                    && !gs.is_kpinned_piece(p_sq, get_delta_between(p_sq, c_sq))) {
                if (counter.has_space()) {
                    Move m{p_sq, c_sq, 0};
                    m.set_score(defend_score(gs.board.get(s)));
                    moves[counter.inc()] = m;
                } else {
                    return;
                }
            }
        }
    }

    Square temp;

    // knights
    for (int i = 0; i < 8; ++i) {

        // first get the square it would be defending from
        const Square defend_from_square = mksq(s.x + XKN[i], s.y + YKN[i]);
        if (val(defend_from_square) && gs.board.get(defend_from_square) == EMPTY) {

            // then check for knights which can move to that square
            for (int j = 0; j < 8; ++j) {
                if (val(temp = mksq(s.x + XKN[i] + XKN[j], s.y + YKN[i] + YKN[j]))
                    && (type(gs.board.get(temp)) == KNIGHT)
                    && (colour(gs.board.get(temp)) == defending_colour)
                    && !equal(temp, s)
                    && !gs.is_kpinned_piece(temp, KNIGHT_DELTA)
                    && move_is_safe(gs, Move{temp, defend_from_square, 0})) {
                    if (counter.has_space()) {
                        Move m{temp, defend_from_square, 0};
                        m.set_score(defend_score(gs.board.get(s)));
                        moves[counter.inc()] = m;
                    } else {
                        // no space remaining
                        return;
                    }
                }
            }
        }
    }

    // kings
    for (int i = 0; i < 8; ++i) {

        // check the possible square that a king could defend from
        const Square defend_from_square = mksq(s.x + XD[i], s.y + YD[i]);
        if (val(defend_from_square) && gs.board.get(defend_from_square) == EMPTY) {

            // and check the surrounding squares for kings
            for (int j = 0; j < 8; ++j) {
                if (val(temp = mksq(s.x + XD[i] + XD[j], s.y + YD[i] + YD[j]))
                    && (type(gs.board.get(temp)) == KING)
                    && (colour(gs.board.get(temp)) == defending_colour)
                    && !equal(temp, s)
                    && would_be_safe_for_king_after(gs, defend_from_square, Move{temp, defend_from_square, 0}, defending_colour)) {
                    if (counter.has_space()) {
                        Move m{temp, defend_from_square, 0};
                        m.set_score(defend_score(gs.board.get(s)));
                        moves[counter.inc()] = m;
                    } else {
                        // no space remaining
                        return;
                    }
                }
            }
        }
    }

    // pawns
    Square left_pawn_defence_square;
    Square right_pawn_defence_square;
    if (defending_colour == WHITE) {
        left_pawn_defence_square = mksq(s.x - 1, s.y - 1);
        right_pawn_defence_square = mksq(s.x + 1, s.y - 1);
    } else {
        left_pawn_defence_square = mksq(s.x - 1, s.y + 1);
        right_pawn_defence_square = mksq(s.x + 1, s.y + 1);
    }

    for (int i = 0; i < pieces_point; ++i) {
        // check that squares are empty?
        if (type(gs.board.get(piece_squares[i])) == PAWN) {
            if (val(left_pawn_defence_square)
                && can_move_to_square(gs.board, piece_squares[i], left_pawn_defence_square)
                && move_is_safe(gs, Move{piece_squares[i], left_pawn_defence_square, 0})) {
                Delta mov_dir = get_delta_between(left_pawn_defence_square, piece_squares[i]);
                if (!gs.is_kpinned_piece(piece_squares[i], mov_dir)) {
                    if (counter.has_space()) {
                        Move m{piece_squares[i], left_pawn_defence_square, 0};
                        m.set_score(defend_score(gs.board.get(s)));
                        moves[counter.inc()] = m;
                    } else {
                        // no space remaining
                        return;
                    }
                }
            } else if (val(right_pawn_defence_square)
                       && can_move_to_square(gs.board, piece_squares[i], right_pawn_defence_square)
                       && move_is_safe(gs, Move{piece_squares[i], right_pawn_defence_square, 0})) {
                Delta mov_dir = get_delta_between(right_pawn_defence_square, piece_squares[i]);
                if (!gs.is_kpinned_piece(piece_squares[i], mov_dir)) {
                    if (counter.has_space()) {
                        Move m{piece_squares[i], right_pawn_defence_square, 0};
                        m.set_score(defend_score(gs.board.get(s)));
                        moves[counter.inc()] = m;
                    } else {
                        // no space remaining
                        return;
                    }
                }
            }
        }
    }

}

/**
 * Responder which tries to defend the centre square of the given feature frame.
 */
void defend_centre(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {
    defend_square(gs, ff->centre, moves, counter);
}

/**
 * Responder which tries to defend the secondary square of the given feature frame.
 */
void defend_secondary(const Gamestate & gs, const FeatureFrame * ff, Move * moves, IndexCounter & counter) {
    defend_square(gs, ff->secondary, moves, counter);
}

const Responder defend_centre_resp{
        "defend-centre",
        &defend_centre
};

const Responder defend_secondary_resp{
    "defend-secondary",
    &defend_secondary
};
