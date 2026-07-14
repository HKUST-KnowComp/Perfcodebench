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
    
    // The original code resets 'hash' to 1469598103934665603ULL at the start of every iteration.
    // This means the result of the previous iteration is completely discarded.
    // We only need to compute the hash for the very last iteration.
    
    uint64_t hash = 1469598103934665603ULL;
    
    // The 'expanded' vector is built by iterating through segments.
    // We can compute the hash on-the-fly to avoid O(N) memory allocation and cache misses.
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
        const uint32_t len = offsets[s + 1] - offsets[s];
        const uint64_t val = static_cast<uint64_t>(s);
        
        // Unrolling or using a tight loop for the length of the segment.
        // For very large 'len', this is the bottleneck.
        for (uint32_t i = 0; i < len; ++i) {
            hash ^= val;
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}
