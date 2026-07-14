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
    // Use a vector for O(1) access and cache friendliness.
    // The histogram is bounded by 'bins'.
    std::vector<uint32_t> counts(bins);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset counts for the current iteration
        std::fill(counts.begin(), counts.end(), 0u);

        // Accumulate counts
        for (uint32_t v : values) {
            // Only increment if within the valid bin range
            if (v < bins) {
                ++counts[v];
            }
        }

        // Calculate checksum for this iteration
        uint64_t current_hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < bins; ++b) {
            current_hash = mix(current_hash, static_cast<uint64_t>(counts[b]));
        }
        final_hash = current_hash;
    }

    return final_hash;
}
