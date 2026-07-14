#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)iters;
    (void)dictionary;
    
    // Direct marking array covers all possible uint16_t codes (0-65535).
    // Using a byte array avoids bit-manipulation overhead and is cache-friendly.
    uint8_t seen[65536] = {0};
    
    for (uint16_t c : codes) {
        seen[c] = 1;
    }
    
    uint64_t count = 0;
    for (int i = 0; i < 65536; ++i) {
        count += seen[i];
    }
    return count;
}