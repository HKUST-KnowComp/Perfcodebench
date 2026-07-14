#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t compute_checksum(const std::vector<uint64_t>& sums, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        hash ^= sums[g];
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) return 0;
    
    // Use a vector as a direct-address table for O(1) dense grouping
    std::vector<uint64_t> sums(groups, 0);
    uint64_t last_hash = 0;
    const size_t n = keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the sums vector efficiently
        std::fill(sums.begin(), sums.end(), 0);

        // Perform aggregation
        for (size_t i = 0; i < n; ++i) {
            sums[keys[i]] += static_cast<uint64_t>(values[i]);
        }

        last_hash = compute_checksum(sums, groups);
    }

    return last_hash;
}
