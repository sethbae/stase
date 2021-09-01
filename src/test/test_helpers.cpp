#include <cstring>
#include "test.h"

/*
 * Checks that all strings present in one vector are present in the other, and no more.
 */
bool assert_string_lists_equal(const std::vector<std::string> & a, const std::vector<std::string> & b) {

    if (a.size() != b.size()) { return false; }

    for (const std::string stra : a) {
        bool present = false;
        for (const std::string strb : b) {
            if (std::strcmp(stra.c_str(), strb.c_str()) == 0) {
                present = true;
                break;
            }
        }
        if (!present) {
            return false;
        }
    }

    return true;

}