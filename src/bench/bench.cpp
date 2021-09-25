#include <vector>
#include <string>

#include "bench.h"
#include "puzzle.h"

void retrieve_all_puzzle_fens(std::vector<std::string> & vec) {

    static std::vector<std::string> cache;

    if (cache.empty()) {
        read_all_fens(cache);
    }

    vec = cache;

}
