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
    
    // The baseline code resets the hash to 1469598103934665603ULL at the start of every iteration.
    // Therefore, only the result of the very last iteration is returned.
    // We can skip the first (iters - 1) iterations entirely.
    
    uint64_t hash = 1469598103934665603ULL;
    const size_t n = offsets.size();
    
    if (n < 2) {
        // If there are no segments to expand, the loop over s doesn't run.
        // The hash remains the initial FNV offset basis.
        return hash;
    }

    for (std::size_t s = 0; s + 1 < n; ++s) {
        const uint32_t len = offsets[s + 1] - offsets[s];
        const uint64_t val = static_cast<uint64_t>(s);
        
        // The inner loop performs: hash = mix(hash, s) repeated 'len' times.
        // Since 'mix' is not easily reducible via mathematical properties like associativity 
        // (due to the XOR and multiplication), we must iterate, but we do so without 
        // the overhead of vector allocation and push_back.
        for (uint32_t i = 0; i < len; ++i) {
            hash ^= val;
            hash *= 1099511628211ULL;
        }
    }

    return hash;
}