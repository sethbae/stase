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
 *  This is the Gamestate object which is used to reduce calculations by storing some Bitmaps and other flags about the game.
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
    recalculate_all();
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

Bitmap g_attack_map(const Gamestate & g, bool friendly) {
    Bitmap attack = 0;
    Bitmap map = friendly ? g.foccupy : g.eoccupy;
    for (Bitmap pos = 1; pos != 0; pos <<= 1) {
        if (pos & map) {
            attack |= pos;
        }
    }
    return attack;
}

void Gamestate::recalculate_attacks() {
    Ptype fcolour = board.colour_to_move();
    Ptype ecolour = fcolour == WHITE ? BLACK : WHITE;

    fattack = g_attack_map(* this, fcolour);
    eattack = g_attack_map(* this, ecolour);
}

void Gamestate::recalculate_positions() {

    vacancy = occupancy = foccupy = eoccupy = 0;
    kings = queens = bishops = knights = rooks = pawns = 0;

    int x = 0, y = 0;
    for (Bitmap i = 1; i != 0; i <<= 1) {

        Square pos = mksq(x++, y);

        if (x == 8) {
            x = 0;
            ++y;
        }

        Piece p = board.get(pos);

        if (type(p) != EMPTY) {
            continue;
        }

        if (colour(p) == WHITE) {
            foccupy |= i;
        }

        occupancy |= i;

        switch (type(p)) {

            case KING: kings |= i; break;
            case QUEEN: queens |= i; break;
            case BISHOP: bishops |= i; break;
            case KNIGHT: knights |= i; break;
            case ROOK: rooks |= i; break;
            case PAWN: pawns |= i; break;
            
            default: break;
        }

    }

    // for (int i = 0; i < 8; ++i) {
    //     for (int j = 0; j < 8; ++j) {

    //         Square pos = mksq(i, j);
    //         Piece p = board.get(pos);

    //         if (type(p) != EMPTY) {
    //             if (colour(p) == WHITE) {
    //                 set_square(foccupy, pos);
    //             }

    //             set_square(occupancy, pos);

    //             switch (type(p)) {

    //                 case KING: set_square(kings, pos); break;
    //                 case QUEEN: set_square(queens, pos); break;
    //                 case BISHOP: set_square(bishops, pos); break;
    //                 case KNIGHT: set_square(knights, pos); break;
    //                 case ROOK: set_square(rooks, pos); break;
    //                 case PAWN: set_square(pawns, pos); break;
                    
    //                 default: break;
    //             }
    //         }

    //     }
    // }

    vacancy = ~occupancy;
    eoccupy = ~foccupy & occupancy;
}

// With Ofast it takes about 5 microseconds
void Gamestate::recalculate_all() {
    recalculate_positions();
    // recalculate_attacks();
}

Bitmap invalid = -1;
Bitmap & get_type_map(Gamestate & g, Ptype t) {
    switch(t) {
        case KING: return g.kings;
        case QUEEN: return g.queens;
        case BISHOP: return g.bishops;
        case KNIGHT: return g.knights;
        case ROOK: return g.rooks;
        case PAWN: return g.pawns;
        default: return invalid;
    }
}

void Gamestate::update(Move m) {
    Piece p_from = board.get(m.from);
    Piece p_to = board.get(m.to);

    Bitmap & from_map = get_type_map(* this, type(p_from));
    Bitmap & to_map = get_type_map(* this, type(p_to));

    occupancy &= ~foccupy;

    unset_square(from_map, m.from);
    set_square(from_map, m.to);

    unset_square(foccupy, m.from);
    set_square(foccupy, m.to);

    if (to_map != invalid) {
        unset_square(to_map, m.to);
        unset_square(eoccupy, m.to);
    }

    occupancy |= foccupy;
    vacancy = ~occupancy;
}

bool in_check(Gamestate & g) {
    return g.kings & g.foccupy & g.eattack;
}
