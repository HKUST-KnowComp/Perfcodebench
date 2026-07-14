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
    if (offsets.size() < 2) {
        // If there are no segments to expand, the loop logic implies 
        // the hash is reset to the FNV offset basis each iteration.
        // However, if the inner loop never runs, the hash remains the basis.
        // Based on the baseline, if iters > 0, the last hash value is the basis.
        return (iters > 0) ? 1469598103934665603ULL : 0;
    }

    uint64_t hash = 1469598103934665603ULL;
    
    // The baseline resets the hash to the FNV offset basis at the start of 
    // the inner loop of the outer loop. The final value returned is the 
    // result of the last iteration.
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
            const uint32_t len = offsets[s + 1] - offsets[s];
            const uint64_t val = static_cast<uint64_t>(s);
            // Optimization: Instead of a loop, we can't easily skip the mix 
            // because it's non-associative/non-commutative, but we can 
            // unroll or simply avoid the vector overhead.
            for (uint32_t i = 0; i < len; ++i) {
                hash = mix(hash, val);
            }
        }
    }

    return hash;
}