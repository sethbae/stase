#include "utils.h"

/**
 * Converts for example "e2e4" into a Move from the e2 square to the e4 square.
 * If the string is not valid, then MOVE_SENTINEL will be returned.
 */
Move unpack_four_char_san(const std::string & s) {
    if (s.size() != 4) {
        // invalid string
        return MOVE_SENTINEL;
    }
    return Move{
            stosq(s.substr(0, 2)),
            stosq(s.substr(2, 4))
    };
}
