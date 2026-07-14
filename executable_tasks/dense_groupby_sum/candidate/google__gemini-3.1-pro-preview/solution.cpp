#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

namespace {

/**
 * Computes the FNV-1a-style checksum over the aggregated sums.
 * @param agg Pointer to the array of sums.
 * @param groups Number of groups to process.
 * @return The computed 64-bit checksum.
 */
inline uint64_t compute_checksum(const uint64_t* agg, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (int g = 0; g < groups; ++g) {
        hash ^= agg[g];
        hash *= prime;
    }
    return hash;
}

}  // namespace

/**
 * Optimized group-by sum implementation.
 * Uses a dense array for aggregation since keys are bounded by 'groups'.
 */
uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) return 0;

    // Use a vector as a buffer for the dense keys [0, groups-1].
    // This replaces the expensive std::unordered_map.
    std::vector<uint64_t> agg(groups);
    uint64_t* agg_ptr = agg.data();
    
    const size_t n = keys.size();
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();

    uint64_t last_checksum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the aggregation buffer for each iteration.
        std::memset(agg_ptr, 0, groups * sizeof(uint64_t));

        // Perform the aggregation.
        // This loop is the primary bottleneck; dense array access is O(1).
        for (size_t i = 0; i < n; ++i) {
            agg_ptr[k_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
        }

        // Compute the checksum for the current iteration's results.
        last_checksum = compute_checksum(agg_ptr, groups);
    }

    return last_checksum;
}
