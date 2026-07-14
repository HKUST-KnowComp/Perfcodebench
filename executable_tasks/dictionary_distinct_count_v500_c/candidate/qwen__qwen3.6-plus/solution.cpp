#include "interface.h"
#include <vector>
#include <cstring>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)iters;
    (void)dictionary;
    
    // Reuse a thread-local buffer to avoid allocation overhead.
    // 65536 covers the full range of uint16_t.
    static thread_local uint8_t seen[65536];
    std::memset(seen, 0, sizeof(seen));
    
    uint64_t count = 0;
    for (uint16_t code : codes) {
        if (!seen[code]) {
            seen[code] = 1;
            ++count;
        }
    }
    return count;
}