#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    // Handle negative groups gracefully to match baseline loop behavior
    size_t num_groups = (groups > 0) ? static_cast<size_t>(groups) : 0;
    
    // Use vector for dense keys instead of unordered_map
    // This avoids hashing overhead and pointer chasing
    std::vector<uint64_t> agg(num_groups, 0);
    uint64_t out = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset aggregation buffer efficiently
        std::fill(agg.begin(), agg.end(), 0ULL);

        // Accumulate values
        // Check bounds to ensure safety and match baseline behavior (keys >= groups are ignored by checksum)
        for (std::size_t i = 0; i < keys.size(); ++i) {
            uint32_t k = keys[i];
            uint32_t v = values[i];
            if (k < num_groups) {
                agg[k] += v;
            }
        }

        // Compute checksum (inlined from baseline for performance)
        uint64_t hash = 1469598103934665603ULL;
        for (size_t g = 0; g < num_groups; ++g) {
            hash ^= agg[g];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }
    return out;
}