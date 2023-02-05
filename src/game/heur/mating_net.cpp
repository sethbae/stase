#include "heur.h"
#include "../king_net.hpp"
#include "../gamestate.hpp"

/**
 * @return a fraction between zero and one, where 1 represents a king unable to move, and 0 represents a king with
 *  many flight squares.
 */
float flight_squares_score(const KingNet * king_net) {
    return ((float) (8 - king_net->flight_squares())) / 8.0f;
}

float back_rank_mate_score(const Gamestate & gs, const Colour c) {
    return has_luft(gs, c) ? 0.0f : 1.0f;
}

float rank_file_score(const Square s) {
    static const float scores[8][8] = {
        {3.0f, 2.5f, 2.2f, 2.0f, 2.0f, 2.2f, 2.5f, 3.0f},
        {2.5f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.5f},
        {2.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.2f},
        {2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f},
        {2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f},
        {2.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.2f},
        {2.5f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 2.5f},
        {3.0f, 2.5f, 2.2f, 2.0f, 2.0f, 2.2f, 2.5f, 3.0f}
    };
    return scores[s.x][s.y] / 3.0f;
}

float black_score(const Gamestate & gs) {
    return flight_squares_score(gs.b_king_net) + rank_file_score(gs.b_king) + back_rank_mate_score(gs, BLACK);
}

float white_score(const Gamestate & gs) {
    return flight_squares_score(gs.w_king_net) + rank_file_score(gs.w_king) + back_rank_mate_score(gs, WHITE);
}

float __metrics::__mating_net(const Gamestate & gs) {

    Colour side_to_move = gs.board.get_white() ? WHITE : BLACK;
    bool white_can_play_check = colour_can_play_check(gs, WHITE);
    bool black_can_play_check = colour_can_play_check(gs, BLACK);

    if (!white_can_play_check && !black_can_play_check && !gs.in_check) {
        return 0.0f;
    }

    float tempo_offset = 0.0f;
    if (!gs.in_check) {
        if (side_to_move == WHITE && white_can_play_check) {
            tempo_offset = 0.25f;
        } else if (side_to_move == BLACK && black_can_play_check) {
            tempo_offset = -0.25f;
        }
    }

    return tempo_offset + (0.33f * (black_score(gs) - white_score(gs)));
}
