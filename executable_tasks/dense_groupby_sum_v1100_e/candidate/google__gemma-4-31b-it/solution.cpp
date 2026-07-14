#include "interface.h"

#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    // Use a pre-allocated vector as a direct-address table for O(1) access
    // This avoids the overhead of std::unordered_map (hashing, collisions, allocations)
    std::vector<uint64_t> agg(groups, 0);
    uint64_t final_checksum = 0;
    
    const size_t n = keys.size();
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    uint64_t* agg_ptr = agg.data();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the aggregation array for each iteration
        std::fill(agg_ptr, agg_ptr + groups, 0);

        // Perform the group-by sum
        for (size_t i = 0; i < n; ++i) {
            agg_ptr[k_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
        }

        // Calculate the checksum based on the provided FNV-like logic
        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            hash ^= agg_ptr[g];
            hash *= 1099511628211ULL;
        }
        final_checksum = hash;
    }

    return final_checksum;
}