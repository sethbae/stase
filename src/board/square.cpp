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

void inc_x(Square & s) { ++s.x; }
void dec_x(Square & s) { --s.x; }
void inc_y(Square & s) { ++s.y; }
void dec_y(Square & s) { --s.y; }
void diag_ur(Square & s) { inc_x(s); inc_y(s); }
void diag_ul(Square & s) { dec_x(s); inc_y(s); }
void diag_dr(Square & s) { inc_x(s); dec_y(s); }
void diag_dl(Square & s) { dec_x(s); dec_y(s); }

void reset_x(Square & s) { s.x = 0; }
void reset_y(Square & s) { s.y = 0; }
