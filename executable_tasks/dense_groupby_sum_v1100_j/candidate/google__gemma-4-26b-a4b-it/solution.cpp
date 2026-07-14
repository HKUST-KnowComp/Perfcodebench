#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t compute_checksum(const std::vector<uint64_t>& agg, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        hash ^= agg[g];
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    // Since keys are bounded by 'groups', we use a direct-address table (vector)
    // for O(1) aggregation and optimal cache performance.
    std::vector<uint64_t> agg(groups, 0);
    uint64_t out = 0;
    const std::size_t n = keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the aggregation buffer
        std::fill(agg.begin(), agg.end(), 0);

        // Perform aggregation
        for (std::size_t i = 0; i < n; ++i) {
            agg[keys[i]] += static_cast<uint64_t>(values[i]);
        }

        // Compute checksum for this iteration
        out = compute_checksum(agg, groups);
    }

    return out;
}
