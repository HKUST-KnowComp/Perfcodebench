#include "interface.h"
#include <vector>
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
            uint32_t v = p_ids[i];
            hash ^= (v & 0xFF); hash *= FNV_PRIME;
            hash ^= ((v >> 8) & 0xFF); hash *= FNV_PRIME;
            hash ^= ((v >> 16) & 0xFF); hash *= FNV_PRIME;
            hash ^= ((v >> 24) & 0xFF); hash *= FNV_PRIME;
            
            v = p_ts[i];
            hash ^= (v & 0xFF); hash *= FNV_PRIME;
            hash ^= ((v >> 8) & 0xFF); hash *= FNV_PRIME;
            hash ^= ((v >> 16) & 0xFF); hash *= FNV_PRIME;
            hash ^= ((v >> 24) & 0xFF); hash *= FNV_PRIME;
            
            uint16_t w = p_levels[i];
            hash ^= (w & 0xFF); hash *= FNV_PRIME;
            hash ^= ((w >> 8) & 0xFF); hash *= FNV_PRIME;
            
            w = p_lens[i];
            hash ^= (w & 0xFF); hash *= FNV_PRIME;
            hash ^= ((w >> 8) & 0xFF); hash *= FNV_PRIME;
        }
    }
    return hash;
}