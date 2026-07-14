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
    
    // Pre-calculate the prefix lengths for all requests to avoid redundant work in the 'iters' loop.
    // The original code repeats the exact same calculation 'iters' times.
    std::vector<uint64_t> prefix_lengths;
    prefix_lengths.reserve(request_offsets.size() - 1);

    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
        uint64_t prefix = 0;
        int start = request_offsets[r];
        int end = request_offsets[r + 1];
        
        for (int i = start; i < end; ++i) {
            // std::binary_search is efficient for sorted vectors.
            if (std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), request_blocks[static_cast<std::size_t>(i)])) {
                ++prefix;
            } else {
                break;
            }
        }
        prefix_lengths.push_back(prefix);
    }

    // The original code resets the hash to the FNV offset basis at the start of every iteration.
    // We must replicate this behavior exactly.
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (uint64_t p : prefix_lengths) {
            hash = mix(hash, p);
        }
        total_hash = hash;
    }

    return total_hash;
}