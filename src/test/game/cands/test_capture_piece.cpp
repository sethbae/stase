#include <vector>
#include <board.h>
#include <iostream>
using std::cout;

#include "../../../game/cands/cands.h"
#include "../../test.h"
#include "../../../game/heur/heur.h"

// save some typing!
const int pawn = piece_value(W_PAWN);
const int knight = piece_value(W_KNIGHT);
const int bishop = piece_value(W_BISHOP);
const int rook = piece_value(W_ROOK);
const int queen = piece_value(W_QUEEN);
const int king = piece_value(W_KING);
const int no_piece = piece_value(W_KING) * 2;

const TestSet<ResponderTestCase> cap_test_set = {
        "game-cands-capture-piece",
        {
                // starting position has none
                ResponderTestCase{
                        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        std::vector<FeatureFrame>{
                            FeatureFrame{stosq("e4"), 0, 0, 0}
                        },
                        {},
                },
                // doesn't capture #1
                ResponderTestCase{
                        "8/6b1/8/8/3N4/2P5/8/8 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d4"), 0, pawn, bishop}
                        },
                        {}
                },
                // doesn't capture #2
                ResponderTestCase{
                        "3r4/3q4/8/3P4/8/1B6/8/8 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d5"), 0, bishop, queen}
                        },
                        {}
                },
                // prefers weaker piece #1
                ResponderTestCase{
                        "8/8/8/3r4/2P1P3/8/8/3Q4 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d5"), 0, pawn, rook}
                        },
                        {"c4d5", "e4d5"}
                },
                // prefers weaker piece #2
                ResponderTestCase{
                        "3r4/8/8/5b2/8/3R4/8/8 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("d3"), 0, no_piece, bishop}
                        },
                        {"f5d3"}
                },
                // knight takes
                ResponderTestCase{
                        "2q5/8/1N6/5b2/8/3r4/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("c8"), 0, knight, bishop}
                        },
                        {"b6c8"}
                },
                // bishop doesn't take
                ResponderTestCase{
                        "8/6p1/5p2/3N4/8/8/8/B7 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("f6"), 0, knight, pawn}
                        },
                        {}
                },
                // rook takes
                ResponderTestCase{
                        "8/1r3q2/8/8/8/1Q6/8/8 b - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("b3"), 0, no_piece, rook}
                        },
                        {"b7b3"}
                },
                // queen takes
                ResponderTestCase{
                        "8/5q2/8/8/8/1Q6/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("f7"), 0, queen, no_piece}
                        },
                        {"b3f7"}
                },
                // takes better but defended piece
                ResponderTestCase{
                        "8/8/8/8/1k2K3/1rqR3R/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("c3"), 0, rook, rook}
                        },
                        {"d3c3"}
                },
                // has two options #1
                ResponderTestCase{
                        "8/8/8/8/1k2K3/3R1b1R/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("f3"), 0, rook, no_piece}
                        },
                        {"d3f3", "h3f3"}
                },
                // has two options #2
                ResponderTestCase{
                        "8/8/5N2/8/4q3/2N2k2/8/8 w - - 0 1",
                        std::vector<FeatureFrame>{
                                FeatureFrame{stosq("e4"), 0, knight, no_piece}
                        },
                        {"c3e4", "f6e4"}
                },
        }
};

bool evaluate_test_case_capture(const ResponderTestCase * tc) {
    return evaluate_responder_test_case(&capture_piece, tc);
}

void test_capture_piece() {
    evaluate_test_set(&cap_test_set, &evaluate_test_case_capture);
}
