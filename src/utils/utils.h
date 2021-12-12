#ifndef STASE_UTILS_H
#define STASE_UTILS_H

#include "../board/move.h"

// converts floating point to two decimal places
std::string dp_2(double);

Move unpack_four_char_san(const std::string &);

#endif //STASE_UTILS_H
