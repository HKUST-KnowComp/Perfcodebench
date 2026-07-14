#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    if (iters <= 0) return 0;
    
    const uint64_t fnv_prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;
    const std::size_t n = ids.size();
    
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t id = ids[i];
        hash ^= id & 0xFF; hash *= fnv_prime;
        hash ^= (id >> 8) & 0xFF; hash *= fnv_prime;
        hash ^= (id >> 16) & 0xFF; hash *= fnv_prime;
        hash ^= (id >> 24) & 0xFF; hash *= fnv_prime;
        
        uint32_t t = ts[i];
        hash ^= t & 0xFF; hash *= fnv_prime;
        hash ^= (t >> 8) & 0xFF; hash *= fnv_prime;
        hash ^= (t >> 16) & 0xFF; hash *= fnv_prime;
        hash ^= (t >> 24) & 0xFF; hash *= fnv_prime;
        
        uint16_t lv = levels[i];
        hash ^= lv & 0xFF; hash *= fnv_prime;
        hash ^= (lv >> 8) & 0xFF; hash *= fnv_prime;
        
        uint16_t ln = lens[i];
        hash ^= ln & 0xFF; hash *= fnv_prime;
        hash ^= (ln >> 8) & 0xFF; hash *= fnv_prime;
    }
    
    return hash;
}