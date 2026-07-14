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
    
    // The baseline code resets 'hash' to the FNV offset basis at the start of every iteration.
    // Therefore, the result of all iterations except the last one is discarded.
    // We only need to compute the hash for the final iteration.
    
    uint64_t hash = 1469598103934665603ULL;
    
    if (offsets.size() < 2) {
        // If there are no segments to expand, the loop over s doesn't run.
        // The baseline would return the initial hash value (1469598103934665603ULL).
        return hash;
    }

    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
        const uint32_t len = offsets[s + 1] - offsets[s];
        const uint64_t segment_val = static_cast<uint64_t>(s);
        
        // Optimization: Instead of a loop for 'len' times, we can't easily skip the mix
        // because it's a non-linear FNV-style hash. However, we avoid the vector allocation.
        for (uint32_t i = 0; i < len; ++i) {
            hash = mix(hash, segment_val);
        }
    }

    return hash;
}
