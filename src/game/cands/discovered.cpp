#include "cands.h"
#include "hook.hpp"
#include "../gamestate.hpp"

bool seek_discoveries(Gamestate & gs, const Square s, const int start, const int stop) {

    const Piece p = gs.board.get(s);
    const Colour c = colour(p);

    for (int i = start; i < stop; ++i) {
        const Delta d = D[i];
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
    switch (type(gs.board.get(s))) {
        case BISHOP:
            return seek_discoveries(gs, s, ORTHO_START, ORTHO_STOP);
        case ROOK:
            return seek_discoveries(gs, s, DIAG_START, DIAG_STOP);
        case QUEEN:
            return seek_discoveries(gs, s, ALL_DIRS_START, ALL_DIRS_STOP);
        case KNIGHT:
            return true;
        case PAWN:
            return true;
        case KING:
            return true;
        default:
            return true;
    }
}

const Hook discovered_hook{
    "discovered",
    8,
    &discovered_attack_hook
};
