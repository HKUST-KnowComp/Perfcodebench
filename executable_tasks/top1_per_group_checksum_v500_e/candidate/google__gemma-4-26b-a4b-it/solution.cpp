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
    
    // Use a vector as a direct-address table for O(1) access.
    // This is significantly faster than std::unordered_map for dense integer keys.
    std::vector<uint32_t> best(group_count);
    const size_t n = groups.size();
    const uint32_t* g_ptr = groups.data();
    const uint32_t* v_ptr = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the best values for this iteration
        std::fill(best.begin(), best.end(), 0);

        // Single pass to find max per group
        for (size_t i = 0; i < n; ++i) {
            uint32_t g = g_ptr[i];
            uint32_t v = v_ptr[i];
            if (v > best[g]) {
                best[g] = v;
            }
        }

        // Compute the checksum
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t g = 0; g < group_count; ++g) {
            hash = mix(hash, static_cast<uint64_t>(best[g]));
        }
        
        // The problem implies the hash is returned after all iterations.
        // Based on the baseline, the hash is reset/recalculated per iteration.
        // We return the hash resulting from the final iteration.
    }

    // Re-calculating the final hash to match the baseline's logic structure
    // where the hash is updated per iteration.
    // Note: The baseline logic actually overwrites 'hash' every iteration.
    // To match the baseline exactly, we must ensure the final 'hash' value is correct.
    
    // Re-running the logic to ensure the exact same state as baseline:
    // The baseline: hash = 1469598103934665603ULL; then loop group_count.
    // This means the 'hash' from previous iterations is discarded.
    
    // Let's refine the loop to be exactly equivalent to the baseline.
    uint64_t final_hash = 0;
    std::fill(best.begin(), best.end(), 0);
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(best.begin(), best.end(), 0);
        for (size_t i = 0; i < n; ++i) {
            if (v_ptr[i] > best[g_ptr[i]]) {
                best[g_ptr[i]] = v_ptr[i];
            }
        }
        final_hash = 1469598103934665603ULL;
        for (uint32_t g = 0; g < group_count; ++g) {
            final_hash = mix(final_hash, static_cast<uint64_t>(best[g]));
        }
    }

    return final_hash;
}