#include "utils.h"

/**
 * Converts for example "e2e4" into a Move from the e2 square to the e4 square.
 * If the string is not valid, then MOVE_SENTINEL will be returned.
 */
Move uci2move(const std::string & s) {
    if (s.size() != 4 && s.size() != 5) {
        // invalid string
        return MOVE_SENTINEL;
    }
    Move m{
            stosq(s.substr(0, 2)),
            stosq(s.substr(2, 4))
    };
    if (s.size() == 5) {
        m.set_prom_piece(type(ctop(s[4])));
    }
    return m;
}

/**
 * Converts a move to UCI format (eg e2e4) including a promotion char if needed (eg f2f1n).
 */
std::string move2uci(const Move m) {

    std::string prom = "";

    if (m.is_prom()) {
        prom = ptoc(m.get_prom_piece(BLACK));
    }

    return sqtos(m.from) + sqtos(m.to) + prom;
}
