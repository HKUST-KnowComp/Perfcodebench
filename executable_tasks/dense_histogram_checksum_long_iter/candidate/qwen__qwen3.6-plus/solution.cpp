#include "interface.h"
#include <vector>
#include <cstring>

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
    std::vector<uint32_t> counts(bins, 0);
    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(counts.data(), 0, bins * sizeof(uint32_t));
        
        for (uint32_t v : values) {
            ++counts[v];
        }
        
        hash = FNV_OFFSET;
        for (uint32_t b = 0; b < bins; ++b) {
            hash ^= counts[b];
            hash *= FNV_PRIME;
        }
    }
    return hash;
}
