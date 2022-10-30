#ifndef STASE_PIN_CACHE_HPP
#define STASE_PIN_CACHE_HPP

#include "../board/board.hpp"

class PinCache {

private:
    static const unsigned CAPACITY = 16;
    
    int size;
    Square p_squares[CAPACITY];
    Piece pieces[CAPACITY];
    Delta deltas[CAPACITY];

public:
    PinCache() : size(0) {
        p_squares[0] = SQUARE_SENTINEL;
    }

    inline void add_pin(const Square s, const Piece p, const Delta delta) {

        // add a piece and delta
        p_squares[size] = s;
        pieces[size] = p;
        deltas[size] = delta;

        ++size;
#ifdef SAFE_STACK_VEC
        if (size >= CAPACITY) {
            exit_with_failure("adding more pins to the pin_cache than there is space for");
        }
#endif
        p_squares[size] = SQUARE_SENTINEL;
    }

    inline bool is_pinned(const Square s, const Delta d) const {
        for (int i = 0; i < size; ++i) {
            // check that the square matches
            if (equal(s, p_squares[i])) {
                // check it can move in the direction requested
                MoveType move_type = direction_of_delta(d);
                if (can_move_in_direction(pieces[i], move_type) || (type(pieces[i]) == PAWN && move_type == DIAG)) {
                    Delta dir1 = deltas[i];
                    Delta dir2 = {(SignedByte) -dir1.dx, (SignedByte) -dir1.dy };
                    // and check that it is pinned in that direction or its diametric opposite
                    return !equal(d, dir1) && !equal(d, dir2);
                }
                return true;
            }
        }
        return false;
    }

    inline Delta delta_of_pin(const Square s) const {
        for (int i = 0; i < size; ++i) {
            // check that the square matches
            if (equal(s, p_squares[i])) {
                return deltas[i];
            }
        }
        return INVALID_DELTA;
    }

    inline bool is_pinned(const Square s) const {
        for (int i = 0; i < size; ++i) {
            // check that the square matches
            if (equal(s, p_squares[i])) {
                return true;
            }
        }
        return false;
    }

    inline void remove_pin(const Square s) {
        for (int i = 0; i < size; ++i) {
            if (equal(p_squares[i], s)) {
                // piece was found: move the remainder of the list left one
                for (int j = i + 1; j < size; ++j) {
                    p_squares[j - 1] = p_squares[j];
                    pieces[j - 1] = pieces[j];
                    deltas[j - 1] = deltas[j];
                }
                --size;

#ifdef SAFE_STACK_VEC
                if (size <= 0) {
                    exit_with_failure("removing pin from empty pin_cache");
                }
#endif

                p_squares[size] = SQUARE_SENTINEL;
                return;
            }
        }
    }
};

#endif // STASE_PIN_CACHE_HPP
