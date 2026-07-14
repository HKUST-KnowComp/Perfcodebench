#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
    std::vector<uint32_t> counts(bins, 0);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        if (bins > 0) {
            std::memset(counts.data(), 0, bins * sizeof(uint32_t));
        }
        for (uint32_t v : values) {
            if (v < bins) {
                ++counts[v];
            }
        }
        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < bins; ++b) {
            hash ^= counts[b];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
