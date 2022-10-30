#include "cands.h"
#include "../../game/gamestate.hpp"
#include "responder.hpp"

bool find_pawn_checks(Gamestate & gs, const Square s, const Square k_sq) {

    const int FORWARDS = colour(gs.board.get(s)) == WHITE ? 1 : -1;
    Square left_of_king = mksq(k_sq.x - 1, k_sq.y - FORWARDS);
    Square right_of_king = mksq(k_sq.x + 1, k_sq.y - FORWARDS);

    Move moves_arr[8];
    ptr_vec<Move> moves(moves_arr, 8);
    piecemoves_ignore_check(gs.board, s, moves);

    for (int i = 0; i < moves.size(); ++i) {
        if (equal(left_of_king, moves[i].to) || equal(right_of_king, moves[i].to)) {
            bool result =
                gs.add_frame(
                    check_hook.id,
                    FeatureFrame{s, moves[i].to, 0, 0}
                );
            if (!result) { return false; }
        }
    }
    return true;
}

bool find_knight_checks(Gamestate & gs, const Square s, const Square k_sq) {

    Square temp;
    Piece p = gs.board.get(s);

    for (int i = 0; i < 8; ++i) {

        // consider the first knight move
        const Square check_from_sq = mksq(s.x + XKN[i], s.y + YKN[i]);

        if (val(check_from_sq) && colour(gs.board.get(check_from_sq)) != colour(p)) {

            // then check for a move to 'capture' the king
            for (int j = 0; j < 8; ++j) {
                if (val(temp = mksq(check_from_sq.x + XKN[j], check_from_sq.y + YKN[j]))
                        && equal(temp, k_sq)) {
                    bool result =
                        gs.add_frame(
                            check_hook.id,
                            FeatureFrame{s, check_from_sq, 0, 0}
                        );
                    if (!result) { return false; }
                }
            }
        }
    }
    return true;
}

bool find_sliding_checks(Gamestate & gs, const Square s, const Square k_sq) {

    // find the squares we can attack the king from
    Square cover_squares_arr[8];
    ptr_vec<Square> cover_squares(cover_squares_arr, 8);
    find_sliding_cover_squares(gs, cover_squares, s, k_sq);

    // add a frame for each
    for (int i = 0; i < cover_squares.size(); ++i) {
        Square square_hit =
            gs.first_piece_encountered(
                cover_squares[i], get_delta_between(cover_squares[i], k_sq));
        if (equal(square_hit, k_sq)) {
            bool result =
                gs.add_frame(
                    check_hook.id,
                    FeatureFrame{s, cover_squares[i], 0, 0}
                );
            if (!result) { return false; }
        }
    }

    return true;
}

bool find_discovered_checks(Gamestate & gs, const Square s) {
    if (gs.can_discover_check(s)) {
        return gs.add_frame(check_hook.id, FeatureFrame{s, SQUARE_SENTINEL, 0, 1});
    }
    return true;
}

/**
 * Finds checks in the given gamestate. Adds feature frames to the given vector to record the ones it does find.
 * Finds checks playable by either colour, regardless of whose turn it is. For discovered checks, no destination
 * is chosen: the secondary square is a sentinel and conf2 is used as a boolean to further indicate this.
 * The format of the FeatureFrames is this:
 * -centre: the current square of a piece which can play a check
 * -secondary: the square on which it can play a check, or SQUARE_SENTINEL if it's a discovery
 * -conf1: unused
 * -conf2: 1 if the check is discovered, 0 otherwise
 */
bool find_checks_hook(Gamestate & gs, const Square s) {

    Piece p = gs.board.get(s);
    Square k_sq =
        colour(p) == WHITE
            ? gs.b_king
            : gs.w_king;

    bool result = find_discovered_checks(gs, s);
    if (!result) {
        return false;
    }

    switch (type(p)) {
        case PAWN: return find_pawn_checks(gs, s, k_sq);
        case KNIGHT: return find_knight_checks(gs, s, k_sq);
        case BISHOP:
        case ROOK:
        case QUEEN: return find_sliding_checks(gs, s, k_sq);
    }
    return true;
}

int play_check(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {

    if (ff->conf_2) {
        // discovered check: delegate to different responder
        return play_discovered_resp.resp(gs, ff, moves, idx, end);
    }

    if (idx < end && !gs.is_kpinned_piece(ff->centre, get_delta_between(ff->centre, ff->secondary))) {

        Move m{ff->centre, ff->secondary, 0};
        int score;
        if (move_is_safe(gs, m)) {
            score = safe_check_score(gs.board.get(m.from));
        } else {
            score = unsafe_check_score(gs.board.get(m.from));
        }
        m.set_score(score);

        moves[idx++] = m;
    }
    return idx;
}

const Hook check_hook{
    "check",
    3,
    &find_checks_hook
};

const Responder play_check_resp{
    "check",
    &play_check
};
