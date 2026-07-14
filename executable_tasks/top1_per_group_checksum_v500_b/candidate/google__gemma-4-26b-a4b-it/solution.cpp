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
    // This replaces the unordered_map for O(1) access and cache efficiency.
    std::vector<uint32_t> best(group_count);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the best values for this iteration.
        std::fill(best.begin(), best.end(), 0);

        const size_t n = groups.size();
        const uint32_t* g_ptr = groups.data();
        const uint32_t* v_ptr = values.data();

        // Single pass to find max per group.
        for (size_t i = 0; i < n; ++i) {
            uint32_t g = g_ptr[i];
            uint32_t v = v_ptr[i];
            if (v > best[g]) {
                best[g] = v;
            }
        }

        // Compute the FNV-1a style hash over the dense array.
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t g = 0; g < group_count; ++g) {
            hash = mix(hash, static_cast<uint64_t>(best[g]));
        }
        final_hash = hash;
    }

    return final_hash;
}