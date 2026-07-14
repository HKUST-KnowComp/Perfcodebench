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
    
    // Pre-calculate the number of requests to avoid repeated size() calls
    const size_t num_requests = request_offsets.size() - 1;
    const uint32_t* cache_ptr = sorted_cache_keys.data();
    const size_t cache_size = sorted_cache_keys.size();
    const uint32_t* blocks_ptr = request_blocks.data();
    const int* offsets_ptr = request_offsets.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t r = 0; r < num_requests; ++r) {
            uint64_t prefix = 0;
            const int start = offsets_ptr[r];
            const int end = offsets_ptr[r + 1];
            
            for (int i = start; i < end; ++i) {
                uint32_t target = blocks_ptr[i];
                // Using std::binary_search is O(log N). 
                // For high throughput, we ensure the compiler can inline this.
                if (std::binary_search(cache_ptr, cache_ptr + cache_size, target)) {
                    ++prefix;
                } else {
                    break;
                }
            }
            hash = mix(hash, prefix);
        }
        total_hash = hash;
    }
    return total_hash;
}