#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
    
    // Pre-allocate a vector to act as a dense array for group maxima.
    // This avoids the overhead of std::unordered_map.
    std::vector<uint32_t> best(group_count);
    uint64_t hash = 0;
    const size_t n = groups.size();
    const uint32_t* groups_ptr = groups.data();
    const uint32_t* values_ptr = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the best values for this iteration.
        std::fill(best.begin(), best.end(), 0);

        // Single pass to find the maximum value for each group.
        for (size_t i = 0; i < n; ++i) {
            uint32_t g = groups_ptr[i];
            uint32_t v = values_ptr[i];
            if (v > best[g]) {
                best[g] = v;
            }
        }

        // Compute the FNV-1a style hash over the resulting maxima.
        hash = 1469598103934665603ULL;
        for (uint32_t g = 0; g < group_count; ++g) {
            hash = mix(hash, static_cast<uint64_t>(best[g]));
        }
    }

    return hash;
}