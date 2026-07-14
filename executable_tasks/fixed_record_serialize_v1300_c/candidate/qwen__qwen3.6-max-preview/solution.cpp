#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    uint64_t hash = 0;
    constexpr uint64_t prime = 1099511628211ULL;
    const std::size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t l = levels[i];
            uint16_t ln = lens[i];
            
            hash ^= (id & 0xFF); hash *= prime;
            hash ^= ((id >> 8) & 0xFF); hash *= prime;
            hash ^= ((id >> 16) & 0xFF); hash *= prime;
            hash ^= ((id >> 24) & 0xFF); hash *= prime;
            
            hash ^= (t & 0xFF); hash *= prime;
            hash ^= ((t >> 8) & 0xFF); hash *= prime;
            hash ^= ((t >> 16) & 0xFF); hash *= prime;
            hash ^= ((t >> 24) & 0xFF); hash *= prime;
            
            hash ^= (l & 0xFF); hash *= prime;
            hash ^= ((l >> 8) & 0xFF); hash *= prime;
            
            hash ^= (ln & 0xFF); hash *= prime;
            hash ^= ((ln >> 8) & 0xFF); hash *= prime;
        }
    }
    return hash;
}
