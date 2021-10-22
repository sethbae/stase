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
                || would_be_on_weak_square(gs, piece_squares[i], fork_square)) {
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
                frames[frames_point++] = FeatureFrame{ fork_square, piece_squares[i], 0, 0 };
            }
        }

    }

    record_hook_features(gs, &knight_fork_hook, &frames[0], frames_point);

    // return value required by hook format and should be ignored
    return false;
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

            std::cout << sqtos(first_piece_sq) << ", " << sqtos(second_piece_sq) << "\n";

            // pieces of same colour
            if (colour(gs.board.get(first_piece_sq)) != colour(gs.board.get(second_piece_sq))) { continue; }

            DeltaPair delta = open_path_between(gs.board, first_piece_sq, second_piece_sq);

            // pieces connected by a straight, open path
            if (!is_valid_delta(delta)) { continue; }

            std::cout << "Valid path found\n";

//            std::cout << type(gs.board.get(first_piece_sq)) << " " << type(gs.board.get(second_piece_sq)) << "\n";
//            std::cout << "delta is diag: " << ((direction_of_delta(delta) == DIAG) ? "true\n" : "false\n");
//            std::cout << "first clause: " << can_move_in_direction(type(gs.board.get(first_piece_sq)), direction_of_delta(delta)) << "\n";
//            std::cout << "second clause: " << can_move_in_direction(type(gs.board.get(second_piece_sq)), direction_of_delta(delta)) << "\n";

            // find a piece which can move there!
            for (int k = 0; k < pieces_point; ++k) {

                Piece forking_piece = gs.board.get(piece_squares[k]);
                std::cout << "Potential forker on " << sqtos(piece_squares[k]) << "\n";

                // piece of the right colour
                if (colour(forking_piece) == colour(gs.board.get(first_piece_sq))) { continue; }

                std::cout << "Found forker of correct colour\n";

                // piece which can move in the required direction to fork
                if (!can_move_in_direction(forking_piece, direction_of_delta(delta))) { continue; }

                std::cout << "Found forker which can move correctly\n";

                // check that both pieces are either weak, or more valuable than the forker
                int forkable_pieces = 0;
                if (would_be_weak_if_attacked(gs, first_piece_sq, forking_piece)
                        || piece_value(gs.board.get(first_piece_sq)) > piece_value(forking_piece)) {
                    ++forkable_pieces;
                }
                if (would_be_weak_if_attacked(gs, second_piece_sq, forking_piece)
                    || piece_value(gs.board.get(second_piece_sq)) > piece_value(forking_piece)) {
                    ++forkable_pieces;
                }
                if (forkable_pieces < 2) { continue; }

                std::cout << "Proceeding with forker!\n";

                bool continue_searching = true;
                int x = get_x(first_piece_sq) + delta.xd, y = get_y(first_piece_sq) + delta.yd;

                while (continue_searching && val(temp = mksq(x, y)) && temp != second_piece_sq) {
                    if (alpha_covers(gs.board, piece_squares[k], temp)) {

                        // only queens can possibly have more than one square to move to on the line
                        continue_searching = (type(forking_piece) == QUEEN);

                        if (!would_be_on_weak_square(gs, piece_squares[k], temp)) {
                            // can move safely to the square! go for it.
                            frames[frames_point++] = FeatureFrame{temp, piece_squares[k], 0, 0};
                        }
                    }
                    std::cout << sqtos(temp) << " not viable\n";
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
