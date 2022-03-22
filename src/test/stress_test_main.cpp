#include <iostream>
#include <vector>
#include <csignal>
#include "../game/gamestate.hpp"

#include "test.h"
#include "game.h"
#include "puzzle.h"

bool stress_test_main() {

    bool passed = true;

    std::cout << "Stress testing individual hooks\n";

    std::vector<Gamestate> states;
    puzzle_gamestates(states);

    for (const Hook & h : ALL_HOOKS) {
        passed = stress_test_individual_hook(states, h) && passed;
    }

    passed = stress_test_cands(states) && passed;

    return passed;
}
