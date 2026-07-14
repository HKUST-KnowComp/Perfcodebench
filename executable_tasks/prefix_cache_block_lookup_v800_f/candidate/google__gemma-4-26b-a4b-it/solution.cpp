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
    uint64_t final_hash = 0;
    
    // Pre-calculate the existence of blocks to avoid repeated binary searches
    // However, since request_blocks can be very large, we only search what we need.
    // To optimize for 'long prefixes', we use the fact that sorted_cache_keys is sorted.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        const size_t num_requests = request_offsets.size();
        
        for (std::size_t r = 0; r + 1 < num_requests; ++r) {
            int start = request_offsets[r];
            int end = request_offsets[r + 1];
            uint64_t prefix = 0;
            
            // Optimization: Use binary search but minimize the number of calls.
            // For long prefixes, we are checking consecutive elements in request_blocks.
            for (int i = start; i < end; ++i) {
                uint32_t target = request_blocks[static_cast<std::size_t>(i)];
                
                // std::binary_search is efficient, but we can ensure the compiler
                // inlines it and uses the most efficient instruction sequence.
                if (std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), target)) {
                    ++prefix;
                } else {
                    break;
                }
            }
            hash = mix(hash, prefix);
        }
        final_hash = hash;
    }
    
    return final_hash;
}