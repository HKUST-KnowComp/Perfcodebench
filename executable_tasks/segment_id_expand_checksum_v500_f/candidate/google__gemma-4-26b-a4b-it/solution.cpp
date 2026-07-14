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
    
    // The original code resets the hash to 1469598103934665603ULL at the start of every iteration.
    // This means only the result of the LAST iteration matters.
    // We can skip all previous iterations and just compute the hash for the final one.
    
    uint64_t hash = 1469598103934665603ULL;
    
    if (offsets.size() < 2) {
        // If there are no segments to expand, the loop over 's' won't execute.
        // The hash remains the seed.
        return hash;
    }

    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
        const uint32_t len = offsets[s + 1] - offsets[s];
        const uint64_t val = static_cast<uint64_t>(s);
        // Instead of pushing to a vector, we mix the value 'len' times.
        for (uint32_t i = 0; i < len; ++i) {
            hash = mix(hash, val);
        }
    }

    return hash;
}
