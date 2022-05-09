#include "board.h"
#include "cands.h"
#include "../gamestate.hpp"
#include "responder.hpp"

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

void find_knight_cover(const Gamestate & gs, std::vector<Move> & moves, const Colour c, const Square c_sq) {

    Square temp;

    for (int i = 0; i < 8; ++i) {

        // first get the square it would be defending from
        const Square defend_from_square = mksq(c_sq.x + XKN[i], c_sq.y + YKN[i]);

        if (val(defend_from_square) && gs.board.get(defend_from_square) == EMPTY) {

            // then check for knights which can move to that square
            for (int j = 0; j < 8; ++j) {
                if (val(temp = mksq(c_sq.x + XKN[i] + XKN[j], c_sq.y + YKN[i] + YKN[j]))
                        && (type(gs.board.get(temp)) == KNIGHT)
                        && (colour(gs.board.get(temp)) == c)
                        && !equal(temp, c_sq)) {
                    moves.push_back(Move{temp, defend_from_square, 0});
                }
            }
        }
    }
}

void find_king_cover(const Gamestate & gs, std::vector<Move> & moves, const Colour c, const Square c_sq) {

    Square temp;

    for (int i = 0; i < 8; ++i) {

        // check the possible square that a king could defend from
        const Square defend_from_square = mksq(c_sq.x + XD[i], c_sq.y + YD[i]);
        if (val(defend_from_square) && gs.board.get(defend_from_square) == EMPTY) {

            // and check the surrounding squares for kings
            for (int j = 0; j < 8; ++j) {
                if (val(temp = mksq(c_sq.x + XD[i] + XD[j], c_sq.y + YD[i] + YD[j]))
                        && (type(gs.board.get(temp)) == KING)
                        && (colour(gs.board.get(temp)) == c)
                        && !equal(temp, c_sq)
                        && would_be_safe_king_square(gs, defend_from_square, c)) {
                    moves.push_back(Move{temp, defend_from_square, 0});
                }
            }
        }
    }
}

void find_pawn_cover(const Gamestate & gs, std::vector<Move> & moves, const Colour c, const Square c_sq) {

    Square left_pawn_defence_square;
    Square right_pawn_defence_square;

    if (c == WHITE) {
        left_pawn_defence_square = mksq(c_sq.x - 1, c_sq.y - 1);
        right_pawn_defence_square = mksq(c_sq.x + 1, c_sq.y - 1);
    } else {
        left_pawn_defence_square = mksq(c_sq.x - 1, c_sq.y + 1);
        right_pawn_defence_square = mksq(c_sq.x + 1, c_sq.y + 1);
    }

    if (val(left_pawn_defence_square)) { find_pawn_moves_to(gs, moves, c, left_pawn_defence_square); }
    if (val(right_pawn_defence_square)) { find_pawn_moves_to(gs, moves, c, right_pawn_defence_square); }
}

/**
 * Given a list in which to accumulate possibilities, this finds squares that a sliding piece could
 * move to in order to defend the given cover square (c_sq). This does not check for pins or for
 * square safety: it just finds the moves.
 * This does nothing if given a non sliding piece (king, pawn or knight).
 */
void find_sliding_cover_squares(
        const Gamestate & gs, std::vector<Square> & squares, const Square piece_square, const Square cover_square) {
    switch (type(gs.board.get(piece_square))) {
        case BISHOP: find_bishop_cover(gs, squares, piece_square, cover_square); return;
        case ROOK: find_rook_cover(gs, squares, piece_square, cover_square); return;
        case QUEEN: find_queen_cover(gs, squares, piece_square, cover_square); return;
        default: return;
    }
}

int defend_square(const Gamestate & gs, const Square s, Move * moves, int idx, int end) {

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

    // find sliding piece cover squares
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
        find_sliding_cover_squares(gs, cover_squares, p_sq, s);

        for (const Square & c_sq : cover_squares) {

            if (move_is_safe(gs, Move{p_sq, c_sq, 0})
                    && !gs.is_kpinned_piece(p_sq, get_delta_between(p_sq, c_sq))) {
                if (idx < end) {
                    Move m{p_sq, c_sq, 0};
                    m.set_score(defend_score(gs.board.get(s)));
                    moves[idx++] = m;
                } else {
                    return idx;
                }
            }
        }
    }

    // find non-sliding moves
    std::vector<Move> covering_moves;
    find_knight_cover(gs, covering_moves, defending_colour, s);
    find_king_cover(gs, covering_moves, defending_colour, s);
    find_pawn_cover(gs, covering_moves, defending_colour, s);

    for (Move & m : covering_moves) {
        Delta d = get_delta_between(m.from, m.to);
        if (!gs.is_kpinned_piece(m.from, d) && move_is_safe(gs, m)) {
            if (idx < end) {
                m.set_score(defend_score(gs.board.get(s)));
                moves[idx++] = m;
            } else {
                return idx;
            }
        }
    }
    return idx;
}

/**
 * Responder which tries to defend the centre square of the given feature frame.
 */
int defend_centre(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {
    return defend_square(gs, ff->centre, moves, idx, end);
}

/**
 * Responder which tries to defend the secondary square of the given feature frame.
 */
int defend_secondary(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {
    return defend_square(gs, ff->secondary, moves, idx, end);
}

const Responder defend_centre_resp{
    "defend-centre",
    &defend_centre
};

const Responder defend_secondary_resp{
    "defend-secondary",
    &defend_secondary
};
