#include <iostream>
#include <vector>

#include "test.h"
#include "game.h"
#include "puzzle.h"

void stress_test_main() {

    std::cout << "Stress testing individual hooks\n";

    std::vector<Gamestate> states;
    puzzle_gamestates(states);

    for (const Hook * h : ALL_HOOKS) {
        stress_test_individual_hook(states, h);
    }

    std::cout << "Stress testing candidates\n";
    stress_test_cands(states);

}
