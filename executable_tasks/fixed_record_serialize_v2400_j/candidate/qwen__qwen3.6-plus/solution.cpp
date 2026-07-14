#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const size_t n = ids.size();
    const uint32_t* p_ids = ids.data();
    const uint32_t* p_ts = ts.data();
    const uint16_t* p_levels = levels.data();
    const uint16_t* p_lens = lens.data();
    
    uint64_t hash = 0;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = p_ids[i];
            hash ^= id & 0xFF; hash *= FNV_PRIME;
            hash ^= (id >> 8) & 0xFF; hash *= FNV_PRIME;
            hash ^= (id >> 16) & 0xFF; hash *= FNV_PRIME;
            hash ^= (id >> 24) & 0xFF; hash *= FNV_PRIME;

            uint32_t t = p_ts[i];
            hash ^= t & 0xFF; hash *= FNV_PRIME;
            hash ^= (t >> 8) & 0xFF; hash *= FNV_PRIME;
            hash ^= (t >> 16) & 0xFF; hash *= FNV_PRIME;
            hash ^= (t >> 24) & 0xFF; hash *= FNV_PRIME;

            uint16_t l = p_levels[i];
            hash ^= l & 0xFF; hash *= FNV_PRIME;
            hash ^= (l >> 8) & 0xFF; hash *= FNV_PRIME;

            uint16_t le = p_lens[i];
            hash ^= le & 0xFF; hash *= FNV_PRIME;
            hash ^= (le >> 8) & 0xFF; hash *= FNV_PRIME;
        }
    }
    return hash;
}