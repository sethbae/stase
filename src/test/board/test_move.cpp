#include "../test.h"

struct MoveScoreTestCase {
    Move m;
    int score;
    int inc;
};

bool evaluate_move_score_test_case(const MoveScoreTestCase * tc) {
    Move m = tc->m;
    m.set_score(tc->score);
    if (m.get_score() != tc->score) {
        return false;
    }
    m.inc_score(tc->inc);
    if (m.get_score() != tc->score + tc->inc) {
        return false;
    }
    return true;
}

bool test_move_scores() {

    std::vector<MoveScoreTestCase> cases;

    for (int i = 0; i < 1000; ++i) {
        cases.push_back(MoveScoreTestCase{random_move(), rand() % 16, 0});
        cases.push_back(MoveScoreTestCase{random_move(), rand() % 10, rand() % 6});
    }

    TestSet<MoveScoreTestCase> set{
        "game-board-move-scores",
        cases
    };

    return evaluate_test_set(&set, &evaluate_move_score_test_case);

}
