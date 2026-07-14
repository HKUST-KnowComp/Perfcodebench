#include "interface.h"
#include <vector>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= FNV_PRIME;
    return hash;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (offsets.size() < 2) {
        // If no segments can be formed, the loop runs but 'expanded' is always empty.
        // The baseline logic: hash starts at 0, then inside loop hash = FNV_OFFSET_BASIS, 
        // then loop over empty expanded. So hash remains FNV_OFFSET_BASIS.
        // However, if iters == 0, hash is 0. 
        // Let's follow the baseline logic exactly.
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = FNV_OFFSET_BASIS;
        }
        return hash;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET_BASIS;
        for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
            const uint32_t len = offsets[s + 1] - offsets[s];
            const uint64_t val = static_cast<uint64_t>(s);
            // Instead of expanding into a vector, we update the hash directly.
            // This preserves the exact sequence of operations: hash = mix(hash, value)
            for (uint32_t i = 0; i < len; ++i) {
                hash = mix(hash, val);
            }
        }
    }
    return hash;
}
