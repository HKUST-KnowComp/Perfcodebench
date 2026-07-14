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
    // Use a vector for O(1) access instead of unordered_map
    std::vector<uint32_t> counts(bins, 0);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset counts for each iteration as per baseline behavior
        std::fill(counts.begin(), counts.end(), 0);

        for (uint32_t v : values) {
            // Only increment if within bin bounds to preserve correctness
            if (v < bins) {
                ++counts[v];
            }
        }

        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < bins; ++b) {
            hash = mix(hash, static_cast<uint64_t>(counts[b]));
        }
    }

    return hash;
}
