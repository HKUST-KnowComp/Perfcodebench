#include "interface.h"

#include <vector>
#include <cstring>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    // Use a vector as a direct-address table for O(1) access and cache efficiency
    std::vector<uint64_t> agg(groups, 0);
    uint64_t final_checksum = 0;
    
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    const size_t n = keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the aggregation array for each iteration
        std::fill(agg.begin(), agg.end(), 0);

        // Perform the group-by sum
        for (size_t i = 0; i < n; ++i) {
            agg[k_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
        }

        // Calculate checksum based on the reference logic
        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            hash ^= agg[g];
            hash *= 1099511628211ULL;
        }
        final_checksum = hash;
    }

    return final_checksum;
}