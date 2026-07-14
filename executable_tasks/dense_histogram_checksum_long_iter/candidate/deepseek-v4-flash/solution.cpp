#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
    std::vector<uint32_t> counts(bins, 0);
    uint64_t hash = 0;
    const uint32_t* values_ptr = values.data();
    size_t values_size = values.size();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset counts to zero
        for (uint32_t i = 0; i < bins; ++i) {
            counts[i] = 0;
        }

        // Count values that fall within [0, bins)
        for (size_t i = 0; i < values_size; ++i) {
            uint32_t v = values_ptr[i];
            if (v < bins) {
                ++counts[v];
            }
        }

        // Compute FNV-1a hash over all bin counts
        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < bins; ++b) {
            uint64_t c = counts[b];
            hash ^= c;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}