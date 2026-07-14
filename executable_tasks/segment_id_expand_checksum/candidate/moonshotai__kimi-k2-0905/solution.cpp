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

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        // re-initialise FNV offset basis each iteration to match baseline behaviour
        uint64_t h = 1469598103934665603ULL;
        const std::size_t n = offsets.size();
        for (std::size_t s = 0; s + 1 < n; ++s) {
            const uint32_t len = offsets[s + 1] - offsets[s];
            // fold segment id s into hash exactly len times
            for (uint32_t i = 0; i < len; ++i) {
                h = mix(h, static_cast<uint32_t>(s));
            }
        }
        hash = h; // keep only the last iteration's result, as baseline does
    }
    return hash;
}