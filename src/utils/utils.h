#ifndef STASE_UTILS_H
#define STASE_UTILS_H

#include "board.h"

// converts floating point to two decimal places
std::string dp_2(double);

Move uci2move(const std::string &);
std::string move2uci(const Move);

#endif //STASE_UTILS_H
