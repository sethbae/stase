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
                    || would_be_weak_after_move(gs, fork_square, Move{piece_squares[i], fork_square})) {
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
                        && (would_be_weak_after_move(gs, forked_square, Move{piece_squares[i], fork_square})
                            || piece_value(p) > piece_value(KNIGHT))) {
                    ++forked_count;
                }
            }

            if (forked_count >= 2) {
                frames[frames_point++] = FeatureFrame{ fork_square, piece_squares[i], 0, 0 };
            }
        }

    }

    record_hook_features(gs, &knight_fork_hook, &frames[0], frames_point);

    // return value required by hook format and should be ignored
    return false;
}

/**
 * Checks that playing the fork move does indeed make a threat to each of the forked pieces. 'Threat'
 * is defined as making their square weak or as attacking a piece which is more valuable than the forker.
 */
bool would_be_effective_fork(
        const Gamestate & gs, const Square forked_piece_a, const Square forked_piece_b, const Move fork_move) {
    int forked_pieces = 0;
    if (would_be_weak_after_move(gs, forked_piece_a, fork_move)
            || piece_value(gs.board.get(forked_piece_a)) > piece_value(gs.board.get(fork_move.from))) {
        ++forked_pieces;
    }
    if (would_be_weak_after_move(gs, forked_piece_b, fork_move)
            || piece_value(gs.board.get(forked_piece_b)) > piece_value(gs.board.get(fork_move.from))) {
        ++forked_pieces;
    }
    return forked_pieces == 2;
}

bool find_sliding_forks(const Gamestate & gs, const Square ignored, FeatureFrame * ignored2) {

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
        for (int j = i + 1; j < pieces_point; ++j) {

            // for each pair of distinct pieces
            Square first_piece_sq = piece_squares[i];
            Square second_piece_sq = piece_squares[j];

            // pieces of same colour
            if (colour(gs.board.get(first_piece_sq)) != colour(gs.board.get(second_piece_sq))) { continue; }

            DeltaPair delta = open_path_between(gs.board, first_piece_sq, second_piece_sq);

            // pieces connected by a straight, open path
            if (!is_valid_delta(delta)) { continue; }

//            std::cout << type(gs.board.get(first_piece_sq)) << " " << type(gs.board.get(second_piece_sq)) << "\n";
//            std::cout << "delta is diag: " << ((direction_of_delta(delta) == DIAG) ? "true\n" : "false\n");
//            std::cout << "first clause: " << can_move_in_direction(type(gs.board.get(first_piece_sq)), direction_of_delta(delta)) << "\n";
//            std::cout << "second clause: " << can_move_in_direction(type(gs.board.get(second_piece_sq)), direction_of_delta(delta)) << "\n";

            // find a piece which can move there!
            for (int k = 0; k < pieces_point; ++k) {

                Piece forking_piece = gs.board.get(piece_squares[k]);

                // piece of the right colour
                if (colour(forking_piece) == colour(gs.board.get(first_piece_sq))) { continue; }

                // piece which can move in the required direction to fork
                if (!can_move_in_direction(forking_piece, direction_of_delta(delta))) { continue; }

                bool continue_searching = true;
                int x = get_x(first_piece_sq) + delta.xd, y = get_y(first_piece_sq) + delta.yd;

                while (continue_searching && val(temp = mksq(x, y)) && temp != second_piece_sq) {
                    if (alpha_covers(gs.board, piece_squares[k], temp)) {

                        /*
                         * Check two things of the move:
                         * - that when we fork, each piece is actually threatened (would_be_effective_fork)
                         * - that the piece we fork with isn't moving to a weak square (would_be_weak_after_move)
                         */
                        if (would_be_effective_fork(
                                gs,
                                first_piece_sq,
                                second_piece_sq,
                                Move{piece_squares[k], temp})
                            && !would_be_weak_after_move(gs, temp, Move{piece_squares[k], temp})) {

                            frames[frames_point++] = FeatureFrame{temp, piece_squares[k], 0, 0};

                        }

                        // only queens can possibly have more than one square to move to on the line
                        continue_searching = (type(forking_piece) == QUEEN);

                    }
                    x += delta.xd;
                    y += delta.yd;
                }

            }

        }
    }

    record_hook_features(gs, &sliding_fork_hook, &frames[0], frames_point);

    // return value required by hook format and should be ignored
    return false;

}

int count_forkable_pieces_after(const Gamestate & gs, const Move move, MoveType dir) {

    int idx_start, idx_stop;
    int count = 0;

    if (dir == ORTHO) {
        idx_start = ORTHO_START;
        idx_stop = ORTHO_STOP;
    } else {
        idx_start = DIAG_START;
        idx_stop = DIAG_STOP;
    }

    for (int i = idx_start; i < idx_stop; ++i) {

        Square first_piece_sq = first_piece_encountered(gs.board, move.to, DeltaPair{(Byte) XD[i], (Byte) YD[i]});
        Piece first_p = gs.board.get(first_piece_sq);

        // check that some piece was actually encountered
        if (first_piece_sq == SQUARE_SENTINEL) { continue; }

        // don't fork pieces that can move in this direction
        if (can_move_in_direction(first_p, dir)) { continue; }

        // only fork weak pieces or more valuable ones
        if (piece_value(first_p) > piece_value(gs.board.get(move.from))
                || would_be_weak_after_move(gs, first_piece_sq, move)) {
            ++count;
        }

    }

    return count;

}

bool find_queen_forks(const Gamestate & gs, const Square ignored, FeatureFrame * ignored2) {

    Square temp;

    FeatureFrame frames[64];
    int frames_point = 0;

    Square piece_squares[32];
    int pieces_point = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(temp = mksq(x, y)) != EMPTY) {
                piece_squares[pieces_point++] = temp;
            }
        }
    }

    // find each queen
    for (int i = 0; i < pieces_point; ++i) {

        if (type(gs.board.get(piece_squares[i])) != QUEEN) {
            continue;
        }
        Square queen_sq = piece_squares[i];

        /*
         * For each square the queen can move to, check if it's a fork
         */

        // for each sliding direction
        for (int j = 0; j < 8; ++j) {

            bool cont = true;
            int x = get_x(queen_sq) + XD[j], y = get_y(queen_sq) + YD[j];

            // perform a 'beta' walk, ie empty squares and at most one capture-square
            while (val(temp = mksq(x, y)) && cont) {

                // check for forks:
                // - there isn't a piece on the square of the queen's colour (illegal move)
                // - teh square is safe
                if (colour(gs.board.get(temp)) != colour(gs.board.get(queen_sq))
                        && !would_be_weak_after_move(gs, temp, Move{queen_sq, temp})) {

                    int forked_pieces =
                        count_forkable_pieces_after(gs, Move{queen_sq, temp}, ORTHO)
                        + count_forkable_pieces_after(gs, Move{queen_sq, temp}, DIAG);

                    if (forked_pieces >= 2) {
                        frames[frames_point++] = FeatureFrame{temp, queen_sq, 0, 0};
                    }

                }

                // loop logic: stop if the square had a piece on it, otherwise take another step
                if (gs.board.get(temp) != EMPTY) {
                    cont = false;
                } else {
                    x += XD[j];
                    y += YD[j];
                }
            }
        }

    }

    record_hook_features(gs, &queen_fork_hook, &frames[0], frames_point);

    // return value required by hook format and should be ignored
    return false;

}
