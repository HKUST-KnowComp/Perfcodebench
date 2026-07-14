#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t /*hash_space*/,
    int iters) {

    const uint32_t* cache_b = sorted_cache_keys.data();
    const uint32_t* cache_e = cache_b + sorted_cache_keys.size();
    const uint32_t* req_b  = request_blocks.data();

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        const uint32_t* cache_it = cache_b;
        for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
            const int off0 = request_offsets[r];
            const int off1 = request_offsets[r + 1];
            uint32_t prefix = 0;
            for (int i = off0; i < off1; ++i) {
                uint32_t block = req_b[i];
                // advance cache iterator to >= block
                while (cache_it != cache_e && *cache_it < block) ++cache_it;
                if (cache_it == cache_e || *cache_it != block) break;
                ++prefix;
            }
            hash = mix(hash, prefix);
        }
    }
    return hash;
}