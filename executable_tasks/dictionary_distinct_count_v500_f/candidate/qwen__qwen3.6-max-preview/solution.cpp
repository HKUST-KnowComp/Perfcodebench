#include "interface.h"
#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)dictionary;
    uint64_t seen[1024];
    uint64_t count = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(seen, 0, sizeof(seen));
        uint64_t current_count = 0;
        for (uint16_t c : codes) {
            uint64_t& word = seen[c >> 6];
            uint64_t mask = 1ULL << (c & 63);
            if (!(word & mask)) {
                word |= mask;
                current_count++;
            }
        }
        count = current_count;
    }
    return count;
}