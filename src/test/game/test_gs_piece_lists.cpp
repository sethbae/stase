#include "../test.h"
#include "../../game/gamestate.hpp"

const TestSet<StringTestCase> pin_cache_test_set{
    "game-gs-pin-cache",
    {
        StringTestCase{
            "3rr1k1/p1p2ppp/1qP1pn2/1P2N3/3PP3/3Q1P2/6PP/RR4K1 w - - 3 23",
            {}
        },
        StringTestCase{
            "2r2rk1/5pp1/p3bn1p/8/1P6/1Q1N4/P1B2PPP/qNB1R1K1 w - - 3 21",
            {}
        },
        StringTestCase{
            "4r1k1/2p3pp/5p2/8/b1P1P1Q1/P2q2PP/1B3P2/R3K2R w KQ - 0 30",
            {}
        },
        StringTestCase{
            "3b2k1/R5p1/2p1nnp1/1p1pp3/1P2PpP1/2PP1P1P/3B4/2N3K1 w - - 4 30",
            {}
        },
        StringTestCase{
            "1k1r1r2/1pp2q2/p7/2P1R2p/3nQ3/5N2/PP1B1PPP/R5K1 w - - 0 23",
            {}
        },
        StringTestCase{
            "1r3r1k/pppq2pp/2nb1p2/3Q4/3P4/1NP1PN2/PP4PP/R4RK1 w - - 1 16",
            {}
        },
        StringTestCase{
            "3R4/5kpp/p7/1p2B3/2b5/5PK1/1Pr3PP/8 w - - 7 34",
            {}
        },
        StringTestCase{
            "2r3k1/5pp1/1R4np/8/1P1N4/P3PKPP/1r3P2/R7 w - - 1 34",
            {}
        },
        StringTestCase{
            "6k1/p3bp2/b3p3/1q1pP2p/1P3p2/2Q2P1P/P2N1BP1/6K1 w - - 1 32",
            {}
        },
        StringTestCase{
            "2kr3r/2p2p2/1p4np/pPqBN1p1/P1P1P1b1/6n1/2Q2PPP/R1R3K1 w - - 0 23",
            {}
        },
        StringTestCase{
            "1q1r1r1k/6b1/p1R1Q2p/6p1/Pn6/5BP1/3BPP1P/3R2K1 w - - 1 31",
            {}
        }
    }
};

bool evaluate_pin_cache_test_case(const StringTestCase *tc) {

    std::vector<Square> squares;
    Gamestate gs(tc->fen);

    const Delta DIR_USED = Delta{0, 1};
    const Delta DIR_NOT_USED = Delta{1, 1};

    // find all squares with pieces on
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (gs.board.get(mksq(x, y)) != EMPTY) {
                squares.push_back(mksq(x, y));
            }
        }
    }

    PinCache wpin_cache;
    PinCache bpin_cache;

    // try adding them
    for (Square s : squares) {

        PinCache & pin_cache =
            colour(gs.board.get(s)) == WHITE
                ? wpin_cache
                : bpin_cache;
        pin_cache.add_pin(s, gs.board.get(s), DIR_USED);

        // NOTE: using can_move_in_direction here does not account for pawns in the same way as inside
        // Gamestate::is_kpinned_piece. This might cause problems if more test cases are introduced,
        // but currently is fine. This misalignment is in the manner of testing, not in can_move_in_direction.

        if (can_move_in_direction(gs.board.get(s), DIR_USED)) {
            // piece can move in this direction, so it shouldn't be pinned in the direction we used,
            // but should be pinned in any other direction
            if (pin_cache.is_pinned(s, DIR_USED)) {
                return false;
            } else if (!pin_cache.is_pinned(s, DIR_NOT_USED)) {
                return false;
            }
        } else if (!pin_cache.is_pinned(s, INVALID_DELTA)) {
            // piece can't move in that direction, so pinned should definitely return true
            return false;
        }
    }

    // note: it's nicer leaving them all in the list so that it builds up!

    // and removing them
    for (Square s : squares) {
        PinCache & pin_cache =
            colour(gs.board.get(s)) == WHITE
                ? wpin_cache
                : bpin_cache;
        pin_cache.remove_pin(s);
        if (pin_cache.is_pinned(s, DIR_USED) || pin_cache.is_pinned(s, DIR_NOT_USED)) {
            return false;
        }
    }

    return true;
}

bool test_pin_cache() {
    return evaluate_test_set(&pin_cache_test_set, &evaluate_pin_cache_test_case);
}
