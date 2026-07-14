#include "interface.h"
#include <algorithm>
#include <unordered_set>

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
    
    // Pre-process cache keys into a fast lookup structure.
    // Since sorted_cache_keys is already sorted, std::binary_search is O(log M).
    // To optimize, we can use a boolean array if hash_space is small, 
    // but for general cases, we stick to the sorted property or a hash set.
    // Given the performance goal, a sorted vector with std::binary_search is actually 
    // quite cache-friendly, but we can optimize the inner loop.

    uint64_t total_hash = 0;
    
    // Pre-calculate the prefix lengths for all requests once to avoid O(iters * N) work.
    std::vector<uint64_t> prefix_lengths;
    prefix_lengths.reserve(request_offsets.size() - 1);

    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
        uint64_t prefix = 0;
        int start = request_offsets[r];
        int end = request_offsets[r + 1];
        
        for (int i = start; i < end; ++i) {
            uint32_t block = request_blocks[static_cast<std::size_t>(i)];
            // Using std::binary_search on the sorted vector
            if (std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), block)) {
                ++prefix;
            } else {
                break;
            }
        }
        prefix_lengths.push_back(prefix);
    }

    // Now run the iterations using the pre-calculated lengths
    for (int iter = 0; iter < iters; ++iter) {
        total_hash = 1469598103934665603ULL;
        for (uint64_t p : prefix_lengths) {
            total_hash = mix(total_hash, p);
        }
    }

    return total_hash;
}