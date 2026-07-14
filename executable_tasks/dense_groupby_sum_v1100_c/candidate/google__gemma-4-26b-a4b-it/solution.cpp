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
    if (groups <= 0) return 0;
    
    // Use a vector for O(1) direct indexing instead of unordered_map
    // This provides much better cache locality and avoids hashing overhead
    std::vector<uint64_t> agg(groups, 0);
    uint64_t out = 0;
    const size_t n = keys.size();
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the aggregation array
        std::fill(agg.begin(), agg.end(), 0);

        // Perform the aggregation
        for (size_t i = 0; i < n; ++i) {
            agg[k_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
        }

        // Compute the checksum for the current iteration
        out = compute_checksum(agg, groups);
    }

    return out;
}