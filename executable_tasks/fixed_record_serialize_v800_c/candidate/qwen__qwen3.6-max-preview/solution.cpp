#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    uint64_t hash = 0;
    constexpr uint64_t fnv_offset = 1469598103934665603ULL;
    constexpr uint64_t fnv_prime = 1099511628211ULL;
    const size_t n = ids.size();
    
    const uint32_t* p_ids = ids.data();
    const uint32_t* p_ts = ts.data();
    const uint16_t* p_levels = levels.data();
    const uint16_t* p_lens = lens.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = fnv_offset;
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = p_ids[i];
            hash ^= id & 0xFF; hash *= fnv_prime;
            hash ^= (id >> 8) & 0xFF; hash *= fnv_prime;
            hash ^= (id >> 16) & 0xFF; hash *= fnv_prime;
            hash ^= (id >> 24) & 0xFF; hash *= fnv_prime;

            uint32_t t = p_ts[i];
            hash ^= t & 0xFF; hash *= fnv_prime;
            hash ^= (t >> 8) & 0xFF; hash *= fnv_prime;
            hash ^= (t >> 16) & 0xFF; hash *= fnv_prime;
            hash ^= (t >> 24) & 0xFF; hash *= fnv_prime;

            uint16_t lev = p_levels[i];
            hash ^= lev & 0xFF; hash *= fnv_prime;
            hash ^= (lev >> 8) & 0xFF; hash *= fnv_prime;

            uint16_t ln = p_lens[i];
            hash ^= ln & 0xFF; hash *= fnv_prime;
            hash ^= (ln >> 8) & 0xFF; hash *= fnv_prime;
        }
    }
    return hash;
}
