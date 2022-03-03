#include "board.h"
#include "cands.h"
#include "../gamestate.hpp"
#include "responder.hpp"

bool can_see(const Gamestate & gs, const Square from, const Square to) {

    Delta d = get_delta_between(from, to);
    Square fpe = first_piece_encountered(gs.board, from, d);

//    std::cout << "fpe: " << sqtos(fpe) << "\n";

    if (is_sentinel(fpe)) { return true; }
    if (d.dx == 0) {
        // y is increasing, so we can reach any square on the line with y less than the fpe's, and vice versa
        if (d.dy > 0 && to.y <= fpe.y) { return true; }
        if (d.dy < 0 && to.y >= fpe.y) { return true; }
    } else {
        // x is increasing, so we can reach any square on the line with x less than the fpe's, and vice versa
        if (d.dx > 0 && to.x <= fpe.x) { return true; }
        if (d.dx < 0 && to.x >= fpe.x) { return true; }
    }
    return false;
}

void find_ortho_cover(
        const Gamestate & gs, std::vector<Square> & squares, const Square p_sq, const Square c_sq) {

    if (p_sq.x == c_sq.x || p_sq.y == c_sq.y) {
        // piece already defends
        return;
    }

    Square top_left = mksq(p_sq.x, c_sq.y);
    Square top_right = mksq(c_sq.x, p_sq.y);

//    std::cout << "tl: " << sqtos(top_left) << "\n";
//    std::cout << "tr: " << sqtos(top_right) << "\n";

    if (can_see(gs, p_sq, top_left)) {
        squares.push_back(top_left);
    }
    if (can_see(gs, p_sq, top_right)) {
        squares.push_back(top_right);
    }
    return;
}

int diag_ordinal(const Square s) {
    if ((s.x + (7 - s.y)) % 2 == 0) {
        return (s.x + (7 - s.y)) / 2;
    } else {
        return ((7 - s.x) + s.y) / 2;
    }
}

void find_diag_cover(const Gamestate & gs, std::vector<Square> & squares, const Square p_sq, const Square c_sq) {

    if (collinear_points(p_sq, p_sq, c_sq)) {
        // piece already defends
        return;
    }

    int diag_diff = diag_ordinal(p_sq) - diag_ordinal(c_sq);
    if (diag_diff > 0) {
        // p is on the further diagonal; up-left from it and down-right from the other
        Square corner1 = mksq(p_sq.x - diag_diff, p_sq.y + diag_diff);
        Square corner2 = mksq(p_sq.x + diag_diff, p_sq.y - diag_diff);
        if (val(corner1)) { squares.push_back(corner1); }
        if (val(corner2)) { squares.push_back(corner2); }
    } else {
        // p is on the lesser diagonal; add to it and sub from the other
        Square corner1 = mksq(p_sq.x + diag_diff, p_sq.y - diag_diff);
        Square corner2 = mksq(p_sq.x - diag_diff, p_sq.y + diag_diff);
        if (val(corner1)) { squares.push_back(corner1); }
        if (val(corner2)) { squares.push_back(corner2); }
    }
}

Square find_edge_of_board(const Gamestate & gs, const Square s, const Delta d) {
    if (d.dx == 0) {
        if (d.dy > 0) { return mksq(s.x, 7); }
        else { return mksq(s.x, 0); }
    }
    if (d.dy == 0) {
        if (d.dx > 0) { return mksq(7, s.y); }
        else { return mksq(0, s.y); }
    }
    Square temp = s;
    // TODO be clever here
    while (val(temp.x + d.dx, temp.y + d.dy)) {
        temp.x += d.dx;
        temp.y += d.dy;
    }
    return temp;
}

void find_queen_cover(const Gamestate & gs, std::vector<Square> & squares, const Square q_sq, const Square c_sq) {
    // strategy is to find the segment of emtpy squares leading from c_sq to either a piece or the edge of the board
    for (int i = 0; i < 8; ++i) {

        Delta d = D[i];

        // TODO make this account for x-rays (use a while loop)
        Square segment_end = first_piece_encountered(gs.board, c_sq, d);
        if (is_sentinel(segment_end)) {
            segment_end = find_edge_of_board(gs.board, c_sq, d);
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

void find_cover_squares(
        const Gamestate & gs, std::vector<Square> & squares, const Square piece_square, const Square cover_square) {
    switch (type(gs.board.get(piece_square))) {
        case BISHOP: find_diag_cover(gs, squares, piece_square, cover_square); return;
        case ROOK: find_ortho_cover(gs, squares, piece_square, cover_square); return;
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
 * Walks out from the piece looking for other pieces which can move to the squares encountered
 * and therefore can defend the piece. Does not consider discovered defences or promotions.
 */
void defend_square2(const Gamestate & gs, const Square s, Move * moves, IndexCounter & move_counter) {

    const Board & b = gs.board;
    const Colour defending_colour = b.get_white() ? WHITE : BLACK;

    Square piece_squares[16];
    int pieces_point = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Square temp = mksq(x, y);
            if (colour(b.get(temp)) == defending_colour
                    && !equal(temp, s)) {
                piece_squares[pieces_point++] = temp;
            }
        }
    }

    int x, y;
    Square temp;

    // go through the delta pairs entailing each sliding direction
    MoveType dir = DIAG;
    for (int i = 0; i < 8; ++i) {

        if (i == 4) {
            dir = ORTHO;
        }

        const int x_inc = XD[i], y_inc = YD[i];
        x = get_x(s) + x_inc, y = get_y(s) + y_inc;

        // and go in that direction
        while (val(temp = mksq(x, y))) {

            Piece p = b.get(temp);

            if (p == EMPTY) {
                // check for any piece which can move here
                for (int j = 0; j < pieces_point; ++j) {

                    // check that the piece does not already control the square
                    if (collinear_points(s, temp, piece_squares[j])
                        || ((type(b.get(piece_squares[j])) == QUEEN) && gamma_covers(b, piece_squares[j], s))) {
                        continue;
                    }

                    Delta mov_dir = get_delta_between(temp, piece_squares[j]);

                    if (beta_covers(b, piece_squares[j], temp)
                            && can_move_in_direction(b.get(piece_squares[j]), dir)
                            && move_is_safe(gs, Move{piece_squares[j], temp, 0})
                            && !gs.is_kpinned_piece(piece_squares[j], mov_dir)) {
                        if (move_counter.has_space()) {
                            Move m{piece_squares[j], temp, 0};
                            m.set_score(defend_score(b.get(s)));
                            moves[move_counter.inc()] = m;
                        } else {
                            // no space remaining
                            return;
                        }
                    }
                }
            } else if (!can_move_in_direction(p, dir)) {
                // blocking piece (which cannot move in same direction)
                break;
            }

            // square was empty, continue
            x += x_inc;
            y += y_inc;

        }

    }

    x = get_x(s), y = get_y(s);

    // knights
    for (int i = 0; i < 8; ++i) {

        // first get the square it would be defending from
        const Square defend_from_square = mksq(x + XKN[i], y + YKN[i]);
        if (val(defend_from_square) && b.get(defend_from_square) == EMPTY) {

            // then check for knights which can move to that square
            for (int j = 0; j < 8; ++j) {
                if (val(temp = mksq(x + XKN[i] + XKN[j], y + YKN[i] + YKN[j]))
                        && (type(b.get(temp)) == KNIGHT)
                        && (colour(b.get(temp)) == defending_colour)
                        && !equal(temp, s)
                        && !gs.is_kpinned_piece(temp, KNIGHT_DELTA)
                        && move_is_safe(gs, Move{temp, defend_from_square, 0})) {
                    if (move_counter.has_space()) {
                        Move m{temp, defend_from_square, 0};
                        m.set_score(defend_score(b.get(s)));
                        moves[move_counter.inc()] = m;
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
        const Square defend_from_square = mksq(x + XD[i], y + YD[i]);
        if (val(defend_from_square) && b.get(defend_from_square) == EMPTY) {

            // and check the surrounding squares for kings
            for (int j = 0; j < 8; ++j) {
                if (val(temp = mksq(x + XD[i] + XD[j], y + YD[i] + YD[j]))
                        && (type(b.get(temp)) == KING)
                        && (colour(b.get(temp)) == defending_colour)
                        && !equal(temp, s)
                        && would_be_safe_for_king_after(gs, defend_from_square, Move{temp, defend_from_square, 0}, defending_colour)) {
                    if (move_counter.has_space()) {
                        Move m{temp, defend_from_square, 0};
                        m.set_score(defend_score(b.get(s)));
                        moves[move_counter.inc()] = m;
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
        left_pawn_defence_square = mksq(x - 1, y - 1);
        right_pawn_defence_square = mksq(x + 1, y - 1);
    } else {
        left_pawn_defence_square = mksq(x - 1, y + 1);
        right_pawn_defence_square = mksq(x + 1, y + 1);
    }

    for (int i = 0; i < pieces_point; ++i) {
        // check that squares are empty?
        if (type(b.get(piece_squares[i])) == PAWN) {
            if (val(left_pawn_defence_square)
                    && can_move_to_square(b, piece_squares[i], left_pawn_defence_square)
                    && move_is_safe(gs, Move{piece_squares[i], left_pawn_defence_square, 0})) {
                Delta mov_dir = get_delta_between(left_pawn_defence_square, piece_squares[i]);
                if (!gs.is_kpinned_piece(piece_squares[i], mov_dir)) {
                    if (move_counter.has_space()) {
                        Move m{piece_squares[i], left_pawn_defence_square, 0};
                        m.set_score(defend_score(b.get(s)));
                        moves[move_counter.inc()] = m;
                    } else {
                        // no space remaining
                        return;
                    }
                }
            } else if (val(right_pawn_defence_square)
                        && can_move_to_square(b, piece_squares[i], right_pawn_defence_square)
                        && move_is_safe(gs, Move{piece_squares[i], right_pawn_defence_square, 0})) {
                Delta mov_dir = get_delta_between(right_pawn_defence_square, piece_squares[i]);
                if (!gs.is_kpinned_piece(piece_squares[i], mov_dir)) {
                    if (move_counter.has_space()) {
                        Move m{piece_squares[i], right_pawn_defence_square, 0};
                        m.set_score(defend_score(b.get(s)));
                        moves[move_counter.inc()] = m;
                    } else {
                        // no space remaining
                        return;
                    }
                }
            }
        }
    }

    return;
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
