#include "../test.h"
#include "../../game/gamestate.hpp"
#include "game.h"
#include <string.h>

struct WeakTestCase {

    const std::string fen;
    std::vector<std::string> squares;
    std::vector<std::string> moves;
    std::vector<std::string> colours;
    std::vector<bool> expected_results;

};

struct UnsafeTestCase {

    const std::string fen;
    std::vector<std::string> squares;
    std::vector<std::string> moves;
    std::vector<bool> expected_results;

};

const TestSet<WeakTestCase> weak_test_cases = {
    "game-piece-weak-square",
    {
        WeakTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {"e1", "e8", "a1", "h8", "e4", "e4"},
            {"none", "none", "none", "none", "none", "none"},
            {"white", "black", "white", "black", "white", "black"},
            {false, false, false, false, false, false}
        },
        // lone white pawn
        WeakTestCase{
            "8/8/8/8/3P4/8/8/8 w - - 0 1",
            {"e5", "c5"},
            {"none", "none"},
            {"white", "black"},
            {false, true}
        },
        // lone black pawn
        WeakTestCase{
                "8/8/8/8/3p4/8/8/8 w - - 0 1",
                {"e3", "c3"},
                {"none", "none"},
                {"white", "black"},
                {true, false}
        },
        // more complex control balance
        WeakTestCase{
            "rnbqkbnr/pp3p1p/2pp2p1/3Pp1N1/2P1P3/2N5/PP3PPP/R1BQKB1R b KQkq - 0 5",
            {"e6", "e6"},
            {"none", "none"},
            {"white", "black"},
            {false, false}
        },
        // pawns in x-rays do count #1
        WeakTestCase{
            "8/8/2R5/4b3/3p4/2P5/8/8 w - - 0 1",
            {"c3"},
            {"none"},
            {"white"},
            {true}
        },
        // pawns in x-rays do count #2
        WeakTestCase{
            "3q4/8/8/5n2/3b4/2P5/8/Q2R4 w - - 0 1",
            {"d4"},
            {"none"},
            {"white"},
            {false}
        },
        // pawns in x-rays don't count #1
        WeakTestCase{
            "4r3/2q5/3b4/4p3/8/4nQn1/8/8 w - - 0 1",
            {"e3", "g3"},
            {"none", "none"},
            {"black", "black"},
            {true, true}
        },
        // pawns in x-rays don't count #2
        WeakTestCase{
            "6n1/4r1p1/8/4P3/3B4/8/8/6R1 w - - 0 1",
            {"g7", "g8"},
            {"none", "none"},
            {"black", "black"},
            {false, false}
        },
        // x-ray #1
        WeakTestCase{
            "8/8/8/3Q4/1RR2rr1/8/8/8 w - - 0 1",
            {"d4", "d4"},
            {"none", "none"},
            {"white", "black"},
            {false, true}
        },
        // x-ray #2
        WeakTestCase{
            "8/5q2/2R1b3/1p6/8/3PN3/8/2R5 w - - 0 1",
            {"c4", "c4"},
            {"none", "none"},
            {"white", "black"},
            {false, true}
        },
        // poly x-ray #1
        WeakTestCase{
            "7k/3rR1R1/8/8/8/8/8/8 w - - 0 1",
            {"h7", "h7"},
            {"none", "none"},
            {"white", "black"},
            {false, true}
        },
        // poly x-ray #2
        WeakTestCase{
            "8/8/8/5q2/4B3/3b4/8/8 w - - 0 1",
            {"b1", "b1", "g5", "g5"},
            {"none", "none", "none", "none"},
            {"white", "black", "white", "black"},
            {true, false, true, false}
        },
        // (AFTER MOVE) complex control balance
        WeakTestCase{
            "3r4/2q2n2/6R1/4b3/5P2/8/8/8 w - - 0 1",
            {"d6", "d6"},
            {"f4e5", "f4e5"},
            {"white", "black"},
            {true, false}
        },
        // (AFTER MOVE) x-ray
        WeakTestCase{
            "1r6/8/8/8/2Q5/8/8/7R w - - 0 1",
            {"b4", "b4"},
            {"h1h4", "h1h4"},
            {"white", "black"},
            {false, true}
        },
        // TODO (GM-35) the poly x-ray logic cannot handle this situation.
//        // (AFTER MOVE) poly x-ray
//        WeakTestCase{
//            "8/2q1Qr2/8/8/8/8/8/3R4 w - - 0 1",
//            {"a7", "a7"},
//            {"d1d7", "d1d7"},
//            {"white", "black"},
//            {false, true}
//        },
    }
};

const TestSet<UnsafeTestCase> unsafe_test_cases = {
    "game-piece-unsafe-piece",
    {
        UnsafeTestCase{
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            {"e1", "e8", "a1", "h8", "e4", "e4"},
            {"none", "none", "none", "none", "none", "none"},
            {false, false, false, false, false, false}
        },
        // basic attacked piece
        UnsafeTestCase{
            "3r4/8/8/8/3N4/8/8/8 w - - 0 1",
            {"d4", "d8"},
            {"none", "none"},
            {true, false}
        },
        // complex attacked piece
        UnsafeTestCase{
            "8/1p1RQ3/8/n7/8/5B2/8/1r6 w - - 0 1",
            {"b7"},
            {"none"},
            {true}
        },
        // attacked by lower value
        UnsafeTestCase{
            "r1bqkbnr/p1pppppp/1p2n3/5P2/8/6P1/PPPPP1BP/RNBQK1NR w KQkq - 0 1",
            {"a8", "e6"},
            {"none", "none"},
            {true, true}
        },
        // equal-value under-defended
        UnsafeTestCase{
            "r1bqkbnr/p1p1pppp/1p6/3p4/4P3/2n3P1/PPPPP1BP/RNBQK1NR w KQkq - 0 1",
            {"d5", "e4"},
            {"none", "none"},
            {false, true}
        },
        // totally undefended
        UnsafeTestCase{
            "r1b1kbnr/p1p1pppp/1pB5/3p4/4P3/2n3P1/PPPPP2P/RNBQK1NR w KQkq - 0 1",
            {"e8"},
            {"none"},
            {true}
        },
        // TODO (GM-36) this logic isn't quite working as intended
//        // under-defended, attacked by weaker piece than defended by
//        UnsafeTestCase{
//            "8/3r4/8/3p2q1/8/4N3/B7/8 w - - 0 1",
//            {"d5"},
//            {"none"},
//            {true}
//        },
        // poly x-ray
        UnsafeTestCase{
            "7k/7p/7Q/8/8/7R/8/7q w - - 0 1",
            {"h7"},
            {"none"},
            {true}
        },
        // (AFTER MOVE) undefended
        UnsafeTestCase{
            "8/8/8/8/3n4/8/8/R3K3 w - - 0 1",
            {"a1", "e1"},
            {"d4c2", "d4c2"},
            {true, true}
        },
        // (AFTER MOVE) attacked by lower value
        UnsafeTestCase{
            "4r3/6p1/5n2/8/4P3/8/8/8 w - - 0 1",
            {"f6"},
            {"e4e5"},
            {true}
        },
        // (AFTER MOVE) equal-value under-defended
        UnsafeTestCase{
            "3r4/8/6n1/4B3/8/8/8/4R3 w - - 0 1",
            {"e5"},
            {"d8e8"},
            {true}
        },
        // (AFTER MOVE) attacked by weaker piece than defended
        UnsafeTestCase{
            "3r4/5p2/6n1/4B3/8/8/8/4R3 w - - 0 1",
            {"e5"},
            {"f7f6"},
            {true}
        },
        // (AFTER MOVE) poly x-ray
        UnsafeTestCase{
            "8/8/8/3b4/2q5/7B/P7/K7 w - - 0 1",
            {"a2"},
            {"h3e6"},
            {true}
        }
    }
};

bool evaluate_weak_test_case(const WeakTestCase * tc) {

    const Gamestate gs(tc->fen);

    for (int i = 0; i < tc->squares.size(); ++i) {

        Square s = stosq(tc->squares[i]);

        Move m;
        if (strcmp(tc->moves[i].c_str(), "none") == 0) {
            m = MOVE_SENTINEL;
        } else {
            m = Move{
                stosq(tc->moves[i].substr(0,2)),
                stosq(tc->moves[i].substr(2,4)),
                0
            };
        }

        Colour colour =
            strcmp(tc->colours[i].c_str(), "white") == 0
                ? WHITE
                : BLACK;

        if (is_sentinel(m)) {
            if (weak_square(gs, s, colour) != tc->expected_results[i]) {
                return false;
            }
        } else {
            if (weak_square(gs, s, colour, m) != tc->expected_results[i]) {
                return false;
            }
        }
    }

    return true;
}

bool evaluate_unsafe_test_case(const UnsafeTestCase * tc) {

    const Gamestate gs(tc->fen);

    for (int i = 0; i < tc->squares.size(); ++i) {

        Square s = stosq(tc->squares[i]);

        Move m;
        if (strcmp(tc->moves[i].c_str(), "none") == 0) {
            m = MOVE_SENTINEL;
        } else {
            m = Move{
                stosq(tc->moves[i].substr(0,2)),
                stosq(tc->moves[i].substr(2,4)),
                0
            };
        }

        if (is_sentinel(m)) {
            if (weak_piece(gs, s) != tc->expected_results[i]) {
                return false;
            }
        } else {
            if (weak_piece(gs, s, m) != tc->expected_results[i]) {
                return false;
            }
        }
    }

    return true;
}

bool test_weak_square() {
    return evaluate_test_set(&weak_test_cases, &evaluate_weak_test_case);
}

bool test_unsafe_piece() {
    return evaluate_test_set(&unsafe_test_cases, &evaluate_unsafe_test_case);
}
