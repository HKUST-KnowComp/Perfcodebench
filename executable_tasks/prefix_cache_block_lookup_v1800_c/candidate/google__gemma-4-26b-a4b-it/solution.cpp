#include "interface.h"
#include <vector>
#include <algorithm>
#include <unordered_set>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
    
    // Use a boolean vector for O(1) lookup if hash_space is manageable,
    // otherwise use a sorted vector with std::binary_search (which is already done).
    // However, the bottleneck is the repeated binary search. 
    // Let's use a more efficient lookup: a sorted vector is good for cache, 
    // but we can optimize the inner loop.
    
    // Pre-calculate existence in a way that is fast to access.
    // Since we don't know the size of hash_space, we check if it's small enough for a bitset.
    // If not, we stick to binary search but optimize the loop.
    
    bool use_bitset = (hash_space <= 10000000);
    std::vector<bool> exists_bitset;
    if (use_bitset) {
        exists_bitset.assign(hash_space + 1, false);
        for (uint32_t key : sorted_cache_keys) {
            if (key <= hash_space) exists_bitset[key] = true;
        }
    }

    uint64_t total_hash = 0;
    const size_t num_requests = request_offsets.size() - 1;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t r = 0; r < num_requests; ++r) {
            uint64_t prefix = 0;
            int start = request_offsets[r];
            int end = request_offsets[r + 1];
            
            for (int i = start; i < end; ++i) {
                uint32_t block = request_blocks[static_cast<size_t>(i)];
                bool found = false;
                if (use_bitset) {
                    if (block <= hash_space && exists_bitset[block]) {
                        found = true;
                    }
                } else {
                    found = std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), block);
                }

                if (found) {
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