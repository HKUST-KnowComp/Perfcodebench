#include "interface.h"
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = 0;
    const size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t l = levels[i];
            uint16_t ln = lens[i];
            
            hash ^= static_cast<uint8_t>(id); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(id >> 8); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(id >> 16); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(id >> 24); hash *= FNV_PRIME;
            
            hash ^= static_cast<uint8_t>(t); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(t >> 8); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(t >> 16); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(t >> 24); hash *= FNV_PRIME;
            
            hash ^= static_cast<uint8_t>(l); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(l >> 8); hash *= FNV_PRIME;
            
            hash ^= static_cast<uint8_t>(ln); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(ln >> 8); hash *= FNV_PRIME;
        }
    }
    return hash;
}
