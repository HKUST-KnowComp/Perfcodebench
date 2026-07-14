#include "interface.h"

#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    const std::size_t n = offsets.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (std::size_t s = 0; s + 1 < n; ++s) {
            uint32_t len = offsets[s + 1] - offsets[s];
            uint32_t val = static_cast<uint32_t>(s);
            for (uint32_t i = 0; i < len; ++i) {
                h ^= val;
                h *= prime;
            }
        }
        hash = h;
    }
    return hash;
}
