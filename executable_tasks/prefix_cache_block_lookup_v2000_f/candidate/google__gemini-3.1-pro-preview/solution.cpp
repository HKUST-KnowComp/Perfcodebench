#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

// Use a static bitset to avoid repeated allocations.
// Given the context of 'large key space', we assume keys fit within a reasonable range.
// If hash_space is very large, we fall back to a more memory-efficient approach or use the bitset if possible.
// For this task, we'll use a bitset for the 32-bit range if memory allows, or a optimized search.
// However, a simple bitset for the entire 2^32 range is 512MB. Let's check if we can optimize the search.

} // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {

    // If the cache is empty, all prefixes are 0.
    if (sorted_cache_keys.empty()) {
        uint64_t final_hash = 0;
        size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t hash = 1469598103934665603ULL;
            for (size_t r = 0; r < num_requests; ++r) {
                hash = mix(hash, 0);
            }
            final_hash = hash;
        }
        return final_hash;
    }

    // Optimization: Use a bitset for O(1) lookup if the range is manageable.
    // Since we don't know the exact memory limit, but 512MB is usually okay in performance tasks,
    // we check the max key. If max key is small, bitset is perfect.
    uint32_t max_key = sorted_cache_keys.back();
    
    // If max_key is within a reasonable range (e.g., 128MB bitset range),
    // we use a bitset. Otherwise, we use the sorted property with a more efficient search.
    bool use_bitset = max_key < (1024 * 1024 * 512); // 512M bits = 64MB

    std::vector<uint8_t> bitset;
    if (use_bitset) {
        bitset.assign((max_key >> 3) + 1, 0);
        for (uint32_t key : sorted_cache_keys) {
            bitset[key >> 3] |= (1 << (key & 7));
        }
    }

    uint64_t final_hash = 0;
    const size_t num_requests = request_offsets.size() - 1;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        
        if (use_bitset) {
            for (size_t r = 0; r < num_requests; ++r) {
                uint64_t prefix = 0;
                int start = request_offsets[r];
                int end = request_offsets[r + 1];
                for (int i = start; i < end; ++i) {
                    uint32_t block = request_blocks[i];
                    if (block > max_key || !(bitset[block >> 3] & (1 << (block & 7)))) {
                        break;
                    }
                    ++prefix;
                }
                hash = mix(hash, prefix);
            }
        } else {
            // Fallback to binary search if key space is too sparse/large
            for (size_t r = 0; r < num_requests; ++r) {
                uint64_t prefix = 0;
                int start = request_offsets[r];
                int end = request_offsets[r + 1];
                for (int i = start; i < end; ++i) {
                    uint32_t block = request_blocks[i];
                    if (!std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), block)) {
                        break;
                    }
                    ++prefix;
                }
                hash = mix(hash, prefix);
            }
        }
        final_hash = hash;
    }

    return final_hash;
}
