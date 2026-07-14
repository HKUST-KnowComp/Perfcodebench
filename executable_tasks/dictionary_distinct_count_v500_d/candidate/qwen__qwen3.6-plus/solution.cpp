#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)dictionary;
    (void)iters;
    
    uint64_t seen[1024]{};
    uint64_t count = 0;
    
    for (uint16_t code : codes) {
        uint64_t word = code >> 6;
        uint64_t mask = 1ULL << (code & 63);
        if (!(seen[word] & mask)) {
            seen[word] |= mask;
            ++count;
        }
    }
    return count;
}