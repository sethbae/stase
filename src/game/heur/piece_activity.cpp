#include "board.h"
#include "game.h"
#include "heur.h"


float piece_activity_alpha_metric(const Gamestate & gs) {

    int w_control = 0;
    int b_control = 0;

    Square sq;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {

            sq = mksq(x, y);
            Piece p = gs.board.get(sq);

            if (colour(p) == WHITE) {
                w_control += alpha_control(gs.board, sq);
            } else if (colour(p) == BLACK) {
                b_control += alpha_control(gs.board, sq);
            }
        }
    }

    float w_proportion = 0.0;

    if (w_control + b_control > 0)
        w_proportion = ((float)w_control / (float)(b_control + w_control));

    return -1.0f + 2*w_proportion;

}

float piece_activity_beta_metric(const Gamestate & gs) {

    int w_control = 0;
    int b_control = 0;

    Square sq;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {

            sq = mksq(x, y);
            Piece p = gs.board.get(sq);

            if (colour(p) == WHITE) {
                w_control += beta_control(gs.board, sq);
            } else if (colour(p) == BLACK) {
                b_control += beta_control(gs.board, sq);
            }
        }
    }

    float w_proportion = 0.0;

    if (w_control + b_control > 0)
        w_proportion = ((float)w_control / (float)(b_control + w_control));

    return -1.0f + 2*w_proportion;

}

float piece_activity_gamma_metric(const Gamestate & gs) {

    int w_control = 0;
    int b_control = 0;

    Square sq;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {

            sq = mksq(x, y);
            Piece p = gs.board.get(sq);

            if (colour(p) == WHITE) {
                w_control += gamma_control(gs.board, sq);
            } else if (colour(p) == BLACK) {
                b_control += gamma_control(gs.board, sq);
            }
        }
    }

    float w_proportion = 0.0;

    if (w_control + b_control > 0)
        w_proportion = ((float)w_control / (float)(b_control + w_control));

    return -1.0f + 2*w_proportion;

}
