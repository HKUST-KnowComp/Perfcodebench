#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
    // Use a vector for O(1) access and better cache locality instead of unordered_map
    std::vector<uint32_t> counts(bins, 0);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset counts for the current iteration
        std::fill(counts.begin(), counts.end(), 0);

        // Accumulate histogram
        for (uint32_t v : values) {
            // The problem implies bins are bounded; we assume v < bins based on the original logic
            if (v < bins) {
                ++counts[v];
            }
        }

        // Calculate checksum
        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < bins; ++b) {
            hash = mix(hash, static_cast<uint64_t>(counts[b]));
        }
    }

    return hash;
}
