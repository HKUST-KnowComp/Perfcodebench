#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = 0;
    const size_t n = ids.size();
    const uint32_t* p_ids = ids.data();
    const uint32_t* p_ts = ts.data();
    const uint16_t* p_levels = levels.data();
    const uint16_t* p_lens = lens.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = p_ids[i];
            uint32_t t = p_ts[i];
            uint16_t lvl = p_levels[i];
            uint16_t ln = p_lens[i];
            
            hash ^= (id & 0xFF); hash *= FNV_PRIME;
            hash ^= ((id >> 8) & 0xFF); hash *= FNV_PRIME;
            hash ^= ((id >> 16) & 0xFF); hash *= FNV_PRIME;
            hash ^= (id >> 24); hash *= FNV_PRIME;
            
            hash ^= (t & 0xFF); hash *= FNV_PRIME;
            hash ^= ((t >> 8) & 0xFF); hash *= FNV_PRIME;
            hash ^= ((t >> 16) & 0xFF); hash *= FNV_PRIME;
            hash ^= (t >> 24); hash *= FNV_PRIME;
            
            hash ^= (lvl & 0xFF); hash *= FNV_PRIME;
            hash ^= (lvl >> 8); hash *= FNV_PRIME;
            
            hash ^= (ln & 0xFF); hash *= FNV_PRIME;
            hash ^= (ln >> 8); hash *= FNV_PRIME;
        }
    }
    return hash;
}
