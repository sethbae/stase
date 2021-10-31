#include "board.h"

const unsigned LO4 = 15;
const unsigned HI4 = 240;
const unsigned LO3 = 7;
const unsigned HI3 = 112;

const Byte SHIFT_X = 1;
const Byte SHIFT_Y = 16;
const Byte SHIFT_POSDIAG = SHIFT_X + SHIFT_Y;
const Byte SHIFT_NEGDIAG = SHIFT_X - SHIFT_Y;

const Byte SHIFT_KN1 = SHIFT_X + 2*SHIFT_Y; // +1x, +2y
const Byte SHIFT_KN2 = SHIFT_X - 2*SHIFT_Y; // +1x, -2y
const Byte SHIFT_KN3 = 2*SHIFT_X + SHIFT_Y; // +2x, +1y
const Byte SHIFT_KN4 = 2*SHIFT_X - SHIFT_Y; // +2x, -1y
const Byte SHIFT_KN5 = -SHIFT_X + 2*SHIFT_Y; // -1x, +2y
const Byte SHIFT_KN6 = -SHIFT_X - 2*SHIFT_Y; // -1x, -2y
const Byte SHIFT_KN7 = -2*SHIFT_X + SHIFT_Y; // -2x, +1y
const Byte SHIFT_KN8 = -2*SHIFT_X - SHIFT_Y; // -2x, -1y

void inc_x(Square & s) { s += SHIFT_X; }
void dec_x(Square & s) { s -= SHIFT_X; }
void inc_y(Square & s) { s += SHIFT_Y; }
void dec_y(Square & s) { s -= SHIFT_Y; }
void diag_ur(Square & s) { s += SHIFT_POSDIAG; }
void diag_ul(Square & s) { s -= SHIFT_NEGDIAG; }
void diag_dr(Square & s) { s += SHIFT_NEGDIAG; }
void diag_dl(Square & s) { s -= SHIFT_POSDIAG; }

void reset_x(Square & s) { s &= HI4; }
void reset_y(Square & s) { s &= LO4; }

int get_y(const Square & s) { return s >> 4; }
int get_x(const Square & s) { return s & LO4; }

bool val_y(const Square & s) { return !(s & 128); }
bool val_x(const Square & s) { return !(s & 8); }
bool val(const Square & s) { return !(s & 128) && !(s & 8); }

