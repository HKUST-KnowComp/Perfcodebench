#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    const uint32_t nseg = static_cast<uint32_t>(offsets.size() - 1);
    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t s = 0; s < nseg; ++s) {
            const uint32_t len = offsets[s + 1] - offsets[s];
            for (uint32_t i = 0; i < len; ++i) {
                hash = mix(hash, s);
            }
        }
    }
    return hash;
}