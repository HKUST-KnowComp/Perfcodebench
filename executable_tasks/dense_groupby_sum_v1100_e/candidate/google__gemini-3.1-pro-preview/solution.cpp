#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

namespace {

/**
 * Computes the FNV-1a-style checksum over the aggregated sums.
 * The checksum is calculated for all group IDs from 0 to groups-1.
 */
inline uint64_t compute_checksum(const uint64_t* agg, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        hash ^= agg[g];
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (iters <= 0) return 0;

    // Use a contiguous vector for dense aggregation since keys are bounded by [0, groups).
    // This provides O(1) access and is significantly faster than std::unordered_map.
    std::vector<uint64_t> agg(groups, 0);
    uint64_t last_checksum = 0;

    const size_t n = keys.size();
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    uint64_t* agg_ptr = agg.data();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the aggregation buffer for each iteration.
        std::fill(agg.begin(), agg.end(), 0ULL);

        // Aggregation loop with manual unrolling to improve ILP (Instruction Level Parallelism).
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            uint32_t k0 = k_ptr[i];
            uint32_t v0 = v_ptr[i];
            uint32_t k1 = k_ptr[i + 1];
            uint32_t v1 = v_ptr[i + 1];
            uint32_t k2 = k_ptr[i + 2];
            uint32_t v2 = v_ptr[i + 2];
            uint32_t k3 = k_ptr[i + 3];
            uint32_t v3 = v_ptr[i + 3];

            agg_ptr[k0] += v0;
            agg_ptr[k1] += v1;
            agg_ptr[k2] += v2;
            agg_ptr[k3] += v3;
        }

        // Handle remaining elements.
        for (; i < n; ++i) {
            agg_ptr[k_ptr[i]] += v_ptr[i];
        }

        // Compute the checksum for the current iteration's results.
        last_checksum = compute_checksum(agg_ptr, groups);
    }

    return last_checksum;
}
