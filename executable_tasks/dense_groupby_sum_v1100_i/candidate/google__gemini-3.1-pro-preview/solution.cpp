#include "interface.h"

#include <vector>
#include <cstdint>
#include <algorithm>

namespace {

/**
 * Computes the FNV-1a style checksum over the aggregated sums.
 * The checksum is calculated for every group index from 0 to groups-1.
 */
inline uint64_t compute_checksum(const std::vector<uint64_t>& agg, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (int g = 0; g < groups; ++g) {
        hash ^= agg[g];
        hash *= prime;
    }
    return hash;
}

} // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) return 0;

    // Use a contiguous vector for dense aggregation to ensure O(1) access and cache efficiency.
    std::vector<uint64_t> agg(groups, 0);
    uint64_t last_checksum = 0;
    
    const size_t n = keys.size();
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the aggregation buffer for each iteration.
        std::fill(agg.begin(), agg.end(), 0);

        // Perform the aggregation. 
        // Manual unrolling can help the compiler optimize the pipeline, 
        // though the bottleneck is typically the random-access write to 'agg'.
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            agg[k_ptr[i]]   += v_ptr[i];
            agg[k_ptr[i+1]] += v_ptr[i+1];
            agg[k_ptr[i+2]] += v_ptr[i+2];
            agg[k_ptr[i+3]] += v_ptr[i+3];
        }
        for (; i < n; ++i) {
            agg[k_ptr[i]] += v_ptr[i];
        }

        // Compute the checksum for the current iteration's results.
        last_checksum = compute_checksum(agg, groups);
    }

    return last_checksum;
}
