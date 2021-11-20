#include "game.h"
#include "cands/cands.h"
#include <iostream>
using std::cout;

/*
 * Values for the move diffs (cannot be defined in header).
 */
const int XD[] = {-1, -1, 1, 1, -1, 1, 0, 0};
const int YD[] = {-1, 1, -1, 1, 0, 0, 1, -1};

const int XKN[] = {1, 1, 2, 2, -1, -1, -2, -2};
const int YKN[] = {2, -2, 1, -1, 2, -2, 1, -1};


/**
 *  This is the Gamestate object which is used to reduce calculations by storing some flags about the game.
 *  Each of those attributes should be updated or re-calculated every move.
 */

/**
 * Allocates memory for all the fields in a gamestate which need it.
 */
void alloc(Gamestate * gs) {
    gs->feature_frames = new FeatureFrame*[ALL_HOOKS.size()];
    gs->wpieces = static_cast<Square*> (operator new(sizeof(Square) * 16));
    gs->bpieces = static_cast<Square*> (operator new(sizeof(Square) * 16));

    // set all pointers to null
    for (int i = 0; i < ALL_HOOKS.size(); ++i) {
        gs->feature_frames[i] = nullptr;
    }
}

Gamestate::Gamestate() {
    alloc(this);
}

Gamestate::Gamestate(const Board & b) : board(b) {
    alloc(this);
}

Gamestate::Gamestate(Gamestate && o) {
    this->board = o.board;
    this->feature_frames = o.feature_frames;
    this->wpieces = o.wpieces;
    this->bpieces = o.bpieces;
    o.feature_frames = nullptr;
    o.wpieces = nullptr;
    o.bpieces = nullptr;
}

/**
 * Copy constructor. This is painfully slow and should be avoided wherever possible.
 * Currently, it's one use case is when caching the puzzles read from file: they need
 * to be copied when a second or later benchmark asks for them.
 */
Gamestate::Gamestate(const Gamestate & o) {
    alloc(this);
    this->board = o.board;
    // copy contents of feature frame across
    for (int i = 0; i < ALL_HOOKS.size(); ++i) {
        if (o.feature_frames[i]) {

            FeatureFrame frames[64];

            int j = 0;
            while (!is_sentinel(o.feature_frames[i][j].centre)) {
                frames[j] = o.feature_frames[i][j];
                ++j;
            }

            this->feature_frames[i] = static_cast<FeatureFrame*> (operator new((sizeof(FeatureFrame)) * (j + 1)));
            for (int k = 0; k < j; ++k) {
                this->feature_frames[i][k] = o.feature_frames[i][k];
            }
            *this->feature_frames[j] = FeatureFrame{SQUARE_SENTINEL, 0, 0, 0};
        }
    }
}

Gamestate::~Gamestate() {
    for (int i = 0; i < ALL_HOOKS.size(); ++i) {
        delete feature_frames[i];
    }
    delete[] feature_frames;
    delete wpieces;
    delete bpieces;
}

/**
 * Populate all the information in the caches completely from scratch.
 */
void Gamestate::repopulate_caches() {
    // for each square, pieces
}
