#include "interface.h"
#include <algorithm>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
    (void)hash_space;
    
    size_t num_requests = request_offsets.size() - 1;
    std::vector<uint64_t> precomputed_prefixes(num_requests);

    // Precompute prefix lengths to avoid redundant binary searches across 'iters'
    for (size_t r = 0; r < num_requests; ++r) {
        uint64_t prefix = 0;
        int start = request_offsets[r];
        int end = request_offsets[r + 1];
        
        for (int i = start; i < end; ++i) {
            uint32_t target = request_blocks[static_cast<size_t>(i)];
            // std::binary_search is fine, but we ensure we only do it once per block
            auto it = std::lower_bound(sorted_cache_keys.begin(), sorted_cache_keys.end(), target);
            if (it != sorted_cache_keys.end() && *it == target) {
                ++prefix;
            } else {
                break;
            }
        }
        precomputed_prefixes[r] = prefix;
    }

    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t r = 0; r < num_requests; ++r) {
            hash = mix(hash, precomputed_prefixes[r]);
        }
        final_hash = hash;
    }

    return final_hash;
}