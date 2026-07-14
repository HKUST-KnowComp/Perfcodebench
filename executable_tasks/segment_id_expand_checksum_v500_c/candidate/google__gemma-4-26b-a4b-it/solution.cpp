#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (iters <= 0) return 0;

    uint64_t hash = 0;
    // The original code resets hash = 1469598103934665603ULL at the start of every iteration.
    // This means only the result of the LAST iteration matters.
    // We can skip the first (iters - 1) iterations entirely.
    
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    hash = FNV_OFFSET_BASIS;

    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
        const uint32_t len = offsets[s + 1] - offsets[s];
        const uint64_t val = static_cast<uint64_t>(s);
        // Unrolling or processing the segment
        for (uint32_t i = 0; i < len; ++i) {
            hash = mix(hash, val);
        }
    }

    return hash;
}
