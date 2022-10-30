#include "cands.h"
#include "hook.hpp"
#include "../gamestate.hpp"
#include "responder.hpp"

/**
 * Looks for situations where a piece is blocking a potential discovery. Does not
 * consider pins or how best to profit from the discovery.
 * Format of FeatureFrames written:
 * centre: the piece blocking the discovery
 * secondary: the enemy piece which would be discovered 'against'
 * conf1: int-ised version of the (blocked) attacker's square
 * conf2: 1 if a check is required to play the discovery, 0 otherwise. This is considered to be
 *          when the attacking piece would become unsafe or when the attacked piece could just
 *          trade without losing any material.
 */
bool discovered_attack_hook(Gamestate & gs, const Square s) {

    const Piece p = gs.board.get(s);
    const Colour c = colour(p);
    if (p == EMPTY) { return true; }

    for (int i = 0; i < 8; ++i) {
        const Delta d = D[i];
        if (can_move_in_direction(p, d)) { continue; }

        const Square sq1 = gs.first_piece_encountered(s, d);
        const Square sq2 = gs.first_piece_encountered(s, delta(-d.dx, -d.dy));

        if (is_sentinel(sq1) || is_sentinel(sq2)) { continue; }

        const Piece own_piece = gs.board.get(sq1);
        const Piece enemy_piece = gs.board.get(sq2);

        if (colour(own_piece) != c || !can_move_in_direction(own_piece, d)) { continue; }
        if (colour(enemy_piece) == c) { continue; }

        if (piece_value(enemy_piece) > piece_value(own_piece)) {

            // we know the piece is worth more, so the only case for caution is when our
            // attacking piece would completely hang
            int check_flag;
            if (can_move_in_direction(enemy_piece, d)
                && totally_undefended(gs, c, sq1)) {
                check_flag = 1;
            } else {
                check_flag = 0;
            }

            if (!gs.add_frame(
                    discovered_hook.id,
                    FeatureFrame{
                        s,
                        sq2,
                        sqtoi(sq1),
                        check_flag
                    })) {
                return false;
            }

        } else if (piece_value(enemy_piece) <= piece_value(own_piece)) {
            if (zero_or_worse_control(gs, sq2)) {

                // here the piece is of equal value, so we need to check if it could trade
                int check_flag = can_move_in_direction(enemy_piece, d) ? 1 : 0;
                if (!gs.add_frame(
                        discovered_hook.id,
                        FeatureFrame{
                            s,
                            sq2,
                            sqtoi(sq1),
                            check_flag
                        })) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Discovered checks are found by the gamestate on startup, and FeatureFrames are written in the following format:
 * centre: the piece blocking the discovery
 * secondary: the piece which would give the discovery
 * conf1: the x coord of the delta to the enemy king
 * conf2: the y coord of the delta to the enemy king
 */
bool discovered_check(Gamestate & gs, const Square s) {
    // intentional stub
    return true;
}

/**
 * Responds to a discovered FeatureFrame by attempting to maximise the damage done. If check
 * is required, this will recommend every appropriate check. Otherwise, it will currently
 * desperado the piece.
 * Todo (ST-88): extend this to threaten a valuable piece where possible.
 */
int play_discovered(const Gamestate & gs, const FeatureFrame * ff, Move * moves, int idx, int end) {

    // always do checks
    for (int i = 0; i < MAX_FRAMES && !is_sentinel(gs.frames[check_hook.id][i].centre); ++i) {

        if (equal(ff->centre, gs.frames[check_hook.id][i].centre)
            && !gs.is_kpinned_piece(ff->centre, get_delta_between(ff->centre, gs.frames[check_hook.id][i].secondary))) {

            if (idx < end) {
                Move m{ff->centre, gs.frames[check_hook.id][i].secondary, 0};
                m.set_score(
                    discovered_score(gs.board.get(itosq(ff->conf_1))));
                moves[idx++] = m;
            } else {
                return idx;
            }
        }
    }

    // if check is not required, do desperado as well
    if (ff->conf_2 == 0 && idx < end) {
        return desperado_resp.resp(gs, ff, moves, idx, end);
    }
    return idx;
}

const Hook discovered_hook{
    "discovered",
    8,
    &discovered_attack_hook
};

const Hook discovered_check_hook{
    "discovered-check",
    10,
    &discovered_check
};

const Responder play_discovered_resp{
    "play-discovered",
    &play_discovered
};
