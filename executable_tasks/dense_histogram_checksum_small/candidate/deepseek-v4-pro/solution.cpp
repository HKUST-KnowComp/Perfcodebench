#include "interface.h"

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
    std::vector<uint32_t> counts(bins, 0);
    for (uint32_t v : values) {
        if (v < bins) {
            counts[v]++;
        }
    }
    uint64_t hash;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < bins; ++b) {
            hash ^= counts[b];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}