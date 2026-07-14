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
    uint64_t total_hash = 0;
    
    const size_t num_requests = request_offsets.size() - 1;
    if (num_requests == 0) return 0;

    // Pre-calculate prefix lengths for all requests to avoid redundant work in 'iters' loop
    std::vector<uint64_t> prefix_lengths(num_requests);
    
    for (size_t r = 0; r < num_requests; ++r) {
        uint64_t prefix = 0;
        int start = request_offsets[r];
        int end = request_offsets[r + 1];
        
        for (int i = start; i < end; ++i) {
            uint32_t block = request_blocks[static_cast<size_t>(i)];
            // Using std::binary_search is O(log N). 
            // For large workloads, this is the bottleneck.
            if (std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), block)) {
                ++prefix;
            } else {
                break;
            }
        }
        prefix_lengths[r] = prefix;
    }

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t r = 0; r < num_requests; ++r) {
            hash = mix(hash, prefix_lengths[r]);
        }
        total_hash = hash;
    }

    return total_hash;
}