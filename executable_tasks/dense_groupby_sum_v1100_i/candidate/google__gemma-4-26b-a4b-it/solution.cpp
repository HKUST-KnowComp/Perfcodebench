#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

// Optimized checksum calculation using direct vector access
uint64_t checksum_vector(const std::vector<uint64_t>& agg, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        uint64_t v = agg[g];
        hash ^= v;
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    // Use a vector for O(1) access since keys are bounded by 'groups'
    // This avoids the hashing and collision overhead of std::unordered_map
    std::vector<uint64_t> agg(groups, 0);
    uint64_t out = 0;
    const std::size_t n = keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the aggregation vector efficiently
        std::fill(agg.begin(), agg.end(), 0);

        // Tight loop for aggregation
        for (std::size_t i = 0; i < n; ++i) {
            agg[keys[i]] += static_cast<uint64_t>(values[i]);
        }

        out = checksum_vector(agg, groups);
    }

    return out;
}