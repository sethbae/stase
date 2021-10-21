#include <iostream>

#include "game.h"
#include "cands.h"
#include "../heur/heur.h"

bool find_knight_forks(const Gamestate & gs, const Square ignored, FeatureFrame * ignored2) {

    FeatureFrame frames[64];
    int frames_point = 0;
    Square temp;

    Square piece_squares[32];
    int pieces_point = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(temp = mksq(x, y)) != EMPTY) {
                piece_squares[pieces_point++] = temp;
            }
        }
    }

    for (int i = 0; i < pieces_point; ++i) {

        if (type(gs.board.get(piece_squares[i])) != KNIGHT) { continue; }

        for (int j = 0; j < 8; ++j) {

            Square fork_square = mksq(get_x(piece_squares[i]) + XKN[j], get_y(piece_squares[i]) + YKN[j]);

            if (!val(fork_square)
                    || would_be_weak_square(gs, piece_squares[i], fork_square)) {
                continue;
            }

            int forked_count = 0;

            for (int k = 0; k < 8; ++k) {

                Square forked_square = mksq(get_x(fork_square) + XKN[k], get_y(fork_square) + YKN[k]);

                if (!val(forked_square)
                        || forked_square == piece_squares[i]) {
                    continue;
                }

                Piece p = gs.board.get(forked_square);

                if (colour(p) != colour(gs.board.get(piece_squares[i]))
                        && type(p) != KNIGHT
                        && (would_be_weak_if_attacked(gs, forked_square, gs.board.get(piece_squares[i]))
                            || piece_value(p) > piece_value(KNIGHT))) {
                    ++forked_count;
                }
            }

            if (forked_count >= 2) {
                frames[frames_point++].centre = fork_square;
            }
        }

    }

    record_hook_features(gs, &knight_fork_hook, &frames[0], frames_point);

    // return value required by hook format and should be ignored
    return false;
}
