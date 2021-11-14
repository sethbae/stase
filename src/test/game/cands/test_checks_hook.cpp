#include "../../../game/cands/cands.h"
#include "../../test.h"

TestSet<StringTestCase> check_hook_test_cases = {
        "game-cands-check-hook",
        {
                StringTestCase{
                        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        {}
                },
                // basic bishop
                StringTestCase{
                        "rnbqkbnr/p1p1pppp/1p6/3P4/8/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1",
                        {"f1b5"}
                },
                // basic knight
                StringTestCase{
                        "rnbqkb1r/p1p1pppp/1p6/3P4/3n4/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 1",
                        {"d4c2", "d4f3"}
                },
                // basic rook
                StringTestCase{
                        "5R2/8/3k4/8/3K4/8/7R/8 w - - 0 1",
                        {"h2h6", "f8f6", "f8d8"}
                },
                // basic queen
                StringTestCase{
                        "8/5q2/3k4/8/3K4/8/8/8 w - - 0 1",
                        {"f7f6", "f7f4", "f7f2", "f7d5", "f7c4", "f7a7", "f7g7"}
                },
                // basic pawn
                StringTestCase{
                        "8/8/3k4/8/8/7p/8/6K1 w - - 0 1",
                        {"h3h2"}
                },
                // with capture #1
                StringTestCase{
                        "rnbqkbnr/pppppppp/8/8/8/1B3Q2/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                        {"b3f7", "f3f7"}
                },
                // with capture #2
                StringTestCase{
                        "rnbqkbnr/pppppppp/4r3/8/8/1B6/PPPPQPPP/RNB1KBNR w KQkq - 0 1",
                        {"e6e2"}
                },
                // discovered #1
                StringTestCase{
                        "r1bq1rk1/ppp1n1bp/3p1np1/4p3/2P5/1BN2N2/PP3PP1/R1BQ1RK1 w - - 1 9",
                        {"c4c5", "d1d5"}
                },
                // discovered #2
                StringTestCase{
                        "3r4/pppk2pp/8/8/8/8/PPP3PP/3K4 w - - 0 1",
                        {"d7c8", "d7c6", "d7e8", "d7e7", "d7e6"}
                },
                // puzzle #1
                StringTestCase{
                        "r1bq1r1k/ppp1n2p/3p1n1Q/3Ppp2/4P3/2N2P2/PPP1N2P/R3KBR1 w Q - 4 15",
                        {"h6h7", "h6g7", "h6f8", "h6f6", "g1g8"}
                },
                // puzzle #2
                StringTestCase{
                        "6k1/p4pp1/1p1r3p/8/3Q3P/qP4P1/5PK1/4R3 w - - 0 27",
                        {"e1e8", "d4g7"}
                },
                // puzzle #3
                StringTestCase{
                        "1r2r3/ppkb2Q1/2n1pB2/q2pP3/3P4/1RP5/P4PPP/1R4K1 w - - 7 25",
                        {"b3b7", "g7d7", "f6d8"}
                },
        }
};

bool evaluate_test_case_check_hook(const StringTestCase *tc) {

    Gamestate gs(fen_to_board(tc->fen));

    discover_feature_frames(gs, &check_hook);

    std::vector<std::string> strings;

    for (FeatureFrame* ff = gs.feature_frames[check_hook.id]; ff->centre != SQUARE_SENTINEL; ++ff) {
        strings.push_back(sqtos(ff->centre) + sqtos(ff->secondary));
    }

    return assert_string_lists_equal(strings, tc->expected_results);
}

bool test_check_hook() {
    return evaluate_test_set(&check_hook_test_cases, &evaluate_test_case_check_hook);
}
