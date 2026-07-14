#include "interface.h"

#include <unordered_set>
#include <vector>
#include <string>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters)
{
    std::unordered_set<std::string> seen;
    seen.reserve(codes.size());          // allocate once

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        seen.clear();                    // reuse memory
        for (uint16_t code : codes) {
            seen.insert(dictionary[static_cast<size_t>(code)]);
        }
        count = static_cast<uint64_t>(seen.size());
    }
    return count;
}