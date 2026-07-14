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
    if (offsets.size() < 2) {
        // If no segments can be formed, the loop in the original code 
        // would result in hash = 1469598103934665603ULL (the FNV offset basis)
        // because the 'expanded' vector remains empty.
        return 1469598103934665603ULL;
    }

    // The sequence of 'expanded' values is identical for every iteration.
    // We calculate the hash once and then simply return it.
    uint64_t hash = 1469598103934665603ULL;
    
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
        const uint32_t len = offsets[s + 1] - offsets[s];
        const uint64_t val = static_cast<uint64_t>(s);
        
        // Optimization: Instead of a loop for 'len' iterations, 
        // we can't easily skip the loop because the FNV-1a mix is non-linear.
        // However, we avoid the vector allocation and push_back overhead.
        for (uint32_t i = 0; i < len; ++i) {
            hash = mix(hash, val);
        }
    }

    return hash;
}
