#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    if (iters <= 0) return 0;
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;
    const std::size_t n = ids.size();
    
    for (std::size_t i = 0; i < n; ++i) {
        // Write id (u32) little-endian
        uint32_t v = ids[i];
        hash ^= v & 0xFF; hash *= multiplier;
        hash ^= (v >> 8) & 0xFF; hash *= multiplier;
        hash ^= (v >> 16) & 0xFF; hash *= multiplier;
        hash ^= (v >> 24) & 0xFF; hash *= multiplier;
        
        // Write ts (u32) little-endian
        v = ts[i];
        hash ^= v & 0xFF; hash *= multiplier;
        hash ^= (v >> 8) & 0xFF; hash *= multiplier;
        hash ^= (v >> 16) & 0xFF; hash *= multiplier;
        hash ^= (v >> 24) & 0xFF; hash *= multiplier;
        
        // Write level (u16) little-endian
        uint16_t w = levels[i];
        hash ^= w & 0xFF; hash *= multiplier;
        hash ^= (w >> 8) & 0xFF; hash *= multiplier;
        
        // Write lens (u16) little-endian
        w = lens[i];
        hash ^= w & 0xFF; hash *= multiplier;
        hash ^= (w >> 8) & 0xFF; hash *= multiplier;
    }
    
    return hash;
}
