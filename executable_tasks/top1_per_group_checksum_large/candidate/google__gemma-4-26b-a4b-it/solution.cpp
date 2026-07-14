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
    
    // Since group IDs are dense and bounded by group_count, 
    // a vector is significantly faster than an unordered_map.
    std::vector<uint32_t> best(group_count);
    const std::size_t n = groups.size();
    const uint32_t* groups_ptr = groups.data();
    const uint32_t* values_ptr = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the best values for this iteration
        std::fill(best.begin(), best.end(), 0);

        // Single pass to find max per group
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t g = groups_ptr[i];
            uint32_t v = values_ptr[i];
            if (v > best[g]) {
                best[g] = v;
            }
        }

        // Compute checksum
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t g = 0; g < group_count; ++g) {
            hash = mix(hash, static_cast<uint64_t>(best[g]));
        }
    }

    // The original code re-initializes hash inside the loop but returns the last one.
    // However, the logic implies the hash is computed per iteration and the final result 
    // is the hash of the last iteration's results.
    // To match the baseline's behavior exactly:
    
    // Re-calculating the final hash to ensure exact match with baseline logic
    // (The baseline resets hash = 1469... inside the loop, so it only returns the hash of the LAST iteration)
    
    std::fill(best.begin(), best.end(), 0);
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t g = groups_ptr[i];
        uint32_t v = values_ptr[i];
        if (v > best[g]) {
            best[g] = v;
        }
    }
    
    uint64_t final_hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
        final_hash = mix(final_hash, static_cast<uint64_t>(best[g]));
    }

    return final_hash;
}