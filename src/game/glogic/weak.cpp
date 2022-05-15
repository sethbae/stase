#include <iostream>
using std::cout;

#include "board.h"
#include "../heur/heur.h"
#include "glogic.h"
#include "../gamestate.hpp"

/**
 * Analyses a SquareControlStatus to detect whether the square is in fact a weak square.
 * If a square is empty, a weak square is defined as one whose control count is strictly
 * unfavourable (ie not zero) for the given colour [c]. If a square is not empty, then
 * weakness is more complex:
 * - attacked by a piece of lower value
 * - attacked by a piece of equal value and not sufficiently defended
 * - attacked by any piece and not defended at all
 * - not sufficiently defended and attacked by a piece of lower value than the weakest defender
 */
bool is_weak_status(const Gamestate & gs, const Square s, const Colour c, SquareControlStatus ss) {

    if (gs.board.get(s) == EMPTY) {
        return c == WHITE
               ? ss.balance < 0
               : ss.balance > 0;
    }

    bool
            totally_undefended,
            attacked_at_all,
            attacked_by_weaker,
            attacked_by_equal,
            under_defended;
    int
            weakest_attacker,
            weakest_defender;

    if (colour(gs.board.get(s)) == WHITE) {

        totally_undefended = (ss.min_w > piece_value(W_KING));
        attacked_at_all = (ss.min_b <= piece_value(B_KING));
        attacked_by_weaker = (ss.min_b < piece_value(gs.board.get(s)));
        attacked_by_equal = (ss.min_b == piece_value(gs.board.get(s)));
        under_defended = (ss.balance < 0);

        weakest_attacker = ss.min_b;
        weakest_defender = ss.min_w;

    } else {

        totally_undefended = (ss.min_b > piece_value(B_KING));
        attacked_at_all = (ss.min_w <= piece_value(W_KING));
        attacked_by_weaker = (ss.min_w < piece_value(gs.board.get(s)));
        attacked_by_equal = (ss.min_w == piece_value(gs.board.get(s)));
        under_defended = (ss.balance > 0);

        weakest_attacker = ss.min_w;
        weakest_defender = ss.min_b;
    }

//    cout << "totally undefended: " << totally_undefended << "\n";
//    cout << "attacked at all: " << attacked_at_all << "\n";
//    cout << "attacked by weaker: " << attacked_by_weaker << "\n";
//    cout << "attacked by equal: " << attacked_by_equal << "\n";
//    cout << "under defended: " << under_defended << "\n";
//    cout << "weakest attacker: " << weakest_attacker << "\n";
//    cout << "weakest defender: " << weakest_defender << "\n";
//    cout << "basic_balance: " << ss.balance << "\n";

    return (
            attacked_by_weaker
            || (attacked_by_equal && under_defended)
            || (attacked_at_all && totally_undefended)
            || (under_defended && (weakest_attacker <= weakest_defender))
    );

}

/**
 * Checks if the given square is weak for either white or black (according to the given colour [c]).
 * If a square is empty, then only the control count is looked at, which must be strictly less or more
 * than zero respectively.
 * If a square is not empty, there is a more complex set of rules. In this case, the colour is not
 * relevant and the return value represents solely the weakness/safety of the piece on the square.
 * The definition of weakness is then:
 * - attacked by a piece of lower value
 * - attacked by a piece of equal value and not sufficiently defended
 * - attacked by any piece and not defended at all
 * - not sufficiently defended and attacked by a piece of lower value than the weakest defender
 * However, there is more complicated logic regarding the interaction of x-rays when pieces are
 * on the same diagonal/orthogonal line.
 */
bool is_weak_square(const Gamestate & gs, const Square s, const Colour c, const bool use_caches) {
    if (!use_caches) {
        return is_weak_status(gs, s, c, __sneaked__evaluate_square_control(gs, s));
    }
    return is_weak_status(gs, s, c, gs.control_cache->get_control_status(s));
}

/**
 * Calculates exactly the same thing as is_weak_square, but after the given move has
 * taken place.
 * Not suitable for kings (use eg is_safe_for_king).
 */
bool would_be_weak_after(const Gamestate & gs, const Square s, const Colour c, const Move m) {

    Piece sneaked = gs.sneak(m);
    bool weak = is_weak_square(gs, s, c, false);
    gs.unsneak(m, sneaked);

    return weak;

}

/**
 * Returns true iff the given square has a weak piece on it. For the definition of weak,
 * see is_weak_square.
 * Not suitable for kings (use eg is_safe_for_king).
 */
bool is_unsafe_piece(const Gamestate & gs, const Square s) {
    return (gs.board.get(s) != EMPTY) && is_weak_square(gs, s, colour(gs.board.get(s)));
}

/**
 * Returns the same as is_unsafe_piece, but after the given move has been played.
 * Not suitable for kings (use eg is_safe_for_king).
 */
bool would_be_unsafe_after(const Gamestate & gs, const Square s, const Move m) {
    return (gs.board.get(s) != EMPTY || equal(s, m.to)) && would_be_weak_after(gs, s, colour(gs.board.get(s)), m);
}

/**
 * Returns false if the move moves a piece onto a square which it isn't safe on. If the square
 * would be safe, returns true.
 */
bool move_is_safe(const Gamestate & gs, const Move m) {

    Piece p = gs.board.get(m.from);
    if (p == EMPTY) { return true; }

    SquareControlStatus status = gs.control_cache->get_control_status(m.to);

    if (colour(p) == WHITE) {

        if (type(p) != PAWN) {
            // pawns don't control the squares they move onto
            status.balance -= 1;
        }

        // if the square is not controlled, the move is safe.
        if (status.min_b > piece_value(B_KING)) { return true; }

        // if the square is only controlled by a king, special care is needed
        if (status.min_b == piece_value(B_KING)) {
            // pawn forward moves do not affect the control, so we can use the min attacker
            if (type(p) == PAWN && m.from.x == m.to.x) {
                return status.min_w <= ATTACKED_BY_PINNED_PIECE;
            }
            return status.balance >= 0;
        }

        // if it's attacked only by more valuable pieces, then:
        if (status.min_b > piece_value(p)) {
            // uncontrolled square: unsafe
            if (status.balance < 0) { return false; }
            // neutral square: safe if defended at all
            if (status.balance == 0) { return status.min_w <= piece_value(W_KING); }
            // controlled square: safe
            return true;
        }

        // if attacked by same piece, then:
        else if (status.min_b == piece_value(p)) {
            // safe unless the square is definitely not controlled
            return status.balance >= 0;
        }

        // if attacked by weaker piece, then unsafe
        else {
            return false;
        }

    } else {

        if (type(p) != PAWN) {
            // pawns don't control the squares they move onto
            status.balance += 1;
        }

        // if the square is not controlled, the move is safe.
        if (status.min_w > piece_value(W_KING)) { return true; }

        // if the square is only controlled by a king, special care is needed
        if (status.min_w == piece_value(W_KING)) {
            if (type(p) == PAWN && m.from.x == m.to.x) {
                return status.min_b <= ATTACKED_BY_PINNED_PIECE;
            }
            return status.balance <= 0;
        }

        // if it's attacked only by more valuable pieces, then:
        if (status.min_w > piece_value(p)) {
            // uncontrolled square: unsafe
            if (status.balance > 0) { return false; }
            // neutral square: safe if defended at all
            if (status.balance == 0) { return status.min_b <= piece_value(B_KING); }
            // controlled square: safe
            return true;
        }

        // if attacked by same piece, then:
        else if (status.min_w == piece_value(p)) {
            // safe unless the square is definitely not controlled
            return status.balance <= 0;
        }

        // if attacked by weaker piece, then unsafe
        else {
            return false;
        }
    }
}
