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
        // If there are no segments to expand, the loop logic implies 
        // the hash is reset to the basis in every iteration.
        // However, the original code only updates hash if expanded is not empty.
        // If offsets.size() < 2, expanded is always empty.
        // The original code: hash = 1469598103934665603ULL; then loop over empty expanded.
        // So it returns 1469598103934665603ULL if iters > 0.
        return (iters > 0) ? FNV_OFFSET_BASIS : 0;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET_BASIS;
        for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
            const uint32_t segment_id = static_cast<uint32_t>(s);
            const uint32_t len = offsets[s + 1] - offsets[s];
            
            // Unrolling or using a simple loop to process the segment expansion
            // without allocating a vector.
            for (uint32_t i = 0; i < len; ++i) {
                hash = mix(hash, static_cast<uint64_t>(segment_id));
            }
        }
    }
    return hash;
}
