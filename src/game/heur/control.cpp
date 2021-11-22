#include "board.h"
#include "game.h"
#include "heur.h"

/**
 * Open files: return the number of squares controlled on the given file (which is presumed
 * to contain no pawns - tho  this is not checked).
 * Essentially this is just 7 squares for whoever controls the file.
 */
int open_line_walk(const Board & b, Square s, Delta d, MoveType dir) {

    int control = 0;

    while (val(s)) {

        Piece p = b.get(s);
        if (can_move_in_direction(p, dir)) {
            control += ((colour(p) == WHITE) ? 1 : -1);
        }
        s.x += d.dx;
        s.y += d.dy;
    }

    // White controls the file
    if (control > 0)
        return 7;

    // Black controls the file
    if (control < 0)
        return -7;

    // nobody does
    return 0;
}

/**
 * Returns an integer representing the difference in controlled squares on a half open file.
 * That is, it counts through the line in  two chunks: before pawn, after pawn. Each half gets
 * its length as score provided that one side has a piece on it (that moves correctly). If a
 * half has pieces of both sides on it, that scores zero. White scores count upwards, black
 * scores count downwards, and the net result is returned.
 */
int half_line_walk(const Board & b, Square s, Delta d, MoveType dir) {

    int score = 0;      // the overall score which we'll return
    int squares_before_pawn = 0;
    int control = 0;    // who controls this run of squares
    int signum = 0;     // -1 -> black does, 0 -> nobody does, 1 -> white does

    // walk up to the pawn
    while (val(s) && (type(b.get(s)) != PAWN)) {
        ++squares_before_pawn;  // counting squares

        Piece p = b.get(s);
        if (can_move_in_direction(p, dir)) { // and control
            control += (colour(p) == WHITE) ? 1 : -1;
        }
        s.x += d.dx;
        s.y += d.dy;
    }

    // calculate contribution to score
    if (control < 0)
        --signum;
    else if (control > 0)
        ++signum;
    score += signum * squares_before_pawn;

    // and now check who controls the other section
    control = 0;
    s.x += d.dx;
    s.y += d.dy;
    while (val(s)) {
        Piece p = b.get(s);
        if (can_move_in_direction(p, dir)) {
            control += (colour(p) == WHITE) ? 1 : -1;
        }
        s.x += d.dx;
        s.y += d.dy;
    }

    // and calculate contribution of second half
    signum = 0;
    if (control < 0)
        --signum;
    else if (control > 0)
        ++signum;
    // we already know the length
    score += signum * (7 - squares_before_pawn);

    return score;
}

/**
 * Counts the number of pawns reachable from the given square according to the step func,
 * ie the number which are on a certain rank or file.
 */
int count_pawns(const Board & b, Square s, Delta d) {

    int sum = 0;

    while (val(s)) {

        Piece p = b.get(s);
        if (type(p) == PAWN) {
            ++sum;
        }
        s.x += d.dx;
        s.y += d.dy;
    }

    return sum;
}

/**
 * Returns a measure of who controls the open lines and diagonals. This checks ranks,
 * files and major diagonals.
 */
float open_line_control_metric(const Board & b) {

    /*

    This tries to measure who controls the position's open lines. It measures this as follows:

        -files are open (no pawns), half open (1 pawn) or closed (2 pawns)
        -files can be controlled by one side or no sides (but not both)
        -different files get different weights (see look up tables)

    It examines all ranks, files, and the six longest diagonals, scoring thus:

        -open: 7 points to whoever controls it
        -half: for each half, 1 point per square to the side who controls it
        -closed: no points

    Points are then scaled by the weights and the whole thing is normalised over 300, a number
    with no real justification but which seems to work ok.

    NB the half open files are scored per file, not per half - so black's points on that file
    are subtracted from white's. That's slightly different to how open files work, which lends
    more weight to the open files - which I think is alright.

    */

    int score = 0;
    Square sq;
    int num_pawns;

    int FILE_VALUES[] = {
            // a  b  c  d  e  f  g  h
            10, 12, 15, 15, 15, 15, 12, 10
    };

    int RANK_VALUES[] = {
            // y =0  1   2   3   4   5   6   7
            15, 15, 10, 10, 10, 10, 15, 15
    };

    int DIAG_VALUES[] = {
            //  a2  a1  b1
            10, 20, 10,   // positive 3 diagonals
            //  b8  a8  a7
            10, 20, 10    // negative 3 diagonals
    };

    // files
    for (int x = 0; x < 8; ++x) {
        sq = mksq(x, 0);
        num_pawns = count_pawns(b, sq, Delta{0, 1});
        if (num_pawns == 0)
            score += open_line_walk(b, sq, Delta{0, 1}, ORTHO) * FILE_VALUES[x];
        else if (num_pawns == 1)
            score += half_line_walk(b, sq, Delta{0, 1}, ORTHO) * FILE_VALUES[x];
    }

    // int file_score = score;
    // cout << "File score: " << score << "\n";

    // ranks
    for (int y = 0; y < 8; ++y) {
        sq = mksq(0, y);
        num_pawns = count_pawns(b, sq, Delta{1, 0});
        if (num_pawns == 0)
            score += open_line_walk(b, sq, Delta{1, 0}, ORTHO) * RANK_VALUES[y];
        else if (num_pawns == 1)
            score += half_line_walk(b, sq, Delta{1, 0}, ORTHO) * RANK_VALUES[y];
    }

    // int rank_score = score;
    // cout << "Rank score: " << score - file_score << "\n";

    // diagonals
    sq = stosq("a2");
    num_pawns = count_pawns(b, sq, Delta{1, 1});
    if (num_pawns == 0)
        score += open_line_walk(b, sq, Delta{1, 1}, DIAG) * DIAG_VALUES[0];
    else if (num_pawns == 1)
        score += half_line_walk(b, sq, Delta{1, 1}, DIAG) * DIAG_VALUES[0];

    sq = stosq("a1");
    num_pawns = count_pawns(b, sq, Delta{1, 1});
    if (num_pawns == 0)
        score += open_line_walk(b, sq, Delta{1, 1}, DIAG) * DIAG_VALUES[1];
    else if (num_pawns == 1)
        score += half_line_walk(b, sq, Delta{1, 1}, DIAG) * DIAG_VALUES[1];

    sq = stosq("b1");
    num_pawns = count_pawns(b, sq, Delta{1, 1});
    if (num_pawns == 0)
        score += open_line_walk(b, sq, Delta{1, 1}, DIAG) * DIAG_VALUES[2];
    else if (num_pawns == 1)
        score += half_line_walk(b, sq, Delta{1, 1}, DIAG) * DIAG_VALUES[2];

    sq = stosq("b8");
    num_pawns = count_pawns(b, sq, Delta{1, -1});
    if (num_pawns == 0)
        score += open_line_walk(b, sq, Delta{1, -1}, DIAG) * DIAG_VALUES[3];
    if (num_pawns == 1)
        score += half_line_walk(b, sq, Delta{1, -1}, DIAG) * DIAG_VALUES[3];

    sq = stosq("a8");
    num_pawns = count_pawns(b, sq, Delta{1, -1});
    if (num_pawns == 0)
        score += open_line_walk(b, sq, Delta{1, -1}, DIAG) * DIAG_VALUES[4];
    if (num_pawns == 1)
        score += half_line_walk(b, sq, Delta{1, -1}, DIAG) * DIAG_VALUES[4];

    sq = stosq("a7");
    num_pawns = count_pawns(b, sq, Delta{1, -1});
    if (num_pawns == 0)
        score += open_line_walk(b, sq, Delta{1, -1}, DIAG) * DIAG_VALUES[5];
    if (num_pawns == 1)
        score += half_line_walk(b, sq, Delta{1, -1}, DIAG) * DIAG_VALUES[5];

    // cout << "Diag score: " << score - rank_score << "\n";

    return ((float)score) / 300.0f;

}

/*
 * Returns a measure of who controls the central squares.
 */
float centre_control_metric(const Board & b) {

    int count = 0;

    for (int i = 0; i < NUM_INNER_CENTRAL_SQUARES; ++i) {

        int control = control_count(b, INNER_CENTRAL_SQUARES[i]);
        if (control > 0) {
            count += 2;
        } else if (control < 0) {
            count -= 2;
        }

    }

    for (int i = 0; i < NUM_OUTER_CENTRAL_SQUARES; ++i) {

        int control = control_count(b, OUTER_CENTRAL_SQUARES[i]);
        if (control > 0) {
            count += 1;
        } else if (control < 0) {
            count -= 1;
        }

    }

    // my thought with normalising over 20 was that 12 + 2x4 = 20, so that represents having
    // control over every square. Obviously this can overflow.
    return ((float) count) / 20.0f;

}
