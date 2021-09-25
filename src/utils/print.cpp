#include <string>
using std::string;
#include <cstring>

#include "utils.h"

/**
 * Converts a double to 2 decimal places.
 */
std::string dp_2(double d) {
    std::string original = std::to_string(d);
    char characters[original.size() + 1];
    std::strcpy(characters, original.c_str());

    int i = 0;
    while (i < original.size() && characters[i] != '.') {
        ++i;
    }
    ++i;

    if (i + 1 >= original.size()) {
        characters[i + 2] = '\0';
        string str(&characters[0]);
        return str;
    }

    if (characters[i + 1] == '9' && characters[i + 2] >= '5') {
        // round up and carry
        if (characters[i] < '9') {
            // does not overflow decimal point
            ++characters[i];
            characters[i + 1] = '0';
        } else {
            // does overflow
            characters[i - 1] = '\0';
            string str(&characters[0]);
            int num = std::stoi(str);  // to int
            ++num;
            str = std::to_string(num);  // to string
            return str + ".00";
        }
    } else if (characters[i + 2] >= '5') {
        // round up
        ++characters[i + 1];
    }

    characters[i + 2] = '\0';
    string str(&characters[0]);
    return str;

}
