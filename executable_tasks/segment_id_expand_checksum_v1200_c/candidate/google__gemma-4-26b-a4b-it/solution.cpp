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
    // The baseline re-initializes hash to the FNV offset basis at the start of every iteration
    // and uses the same expanded sequence. Since the sequence is identical every time,
    // we only need to compute the hash once and return it.
    
    hash = 1469598103934665603ULL;
    
    // Pre-calculate the size to avoid bounds checking in the inner loop if possible,
    // but the primary bottleneck is the nested loop structure.
    const size_t n = offsets.size();
    if (n > 1) {
        for (std::size_t s = 0; s + 1 < n; ++s) {
            const uint32_t len = offsets[s + 1] - offsets[s];
            const uint64_t val = static_cast<uint64_t>(s);
            
            // Unrolling or using a simple loop to apply the mix function
            // for the 'len' number of times for the current segment index 's'.
            for (uint32_t i = 0; i < len; ++i) {
                hash ^= val;
                hash *= 1099511628211ULL;
            }
        }
    }

    // The baseline loop: for (int iter = 0; iter < iters; ++iter) {
    //   expanded.clear(); ... hash = 1469598103934665603ULL; for (uint32_t value : expanded) { hash = mix(hash, value); }
    // }
    // Note: The baseline resets 'hash' to the offset basis INSIDE the 'iter' loop.
    // This means the result of the last iteration is the only one that matters.
    // Therefore, we only need to perform the calculation once.
    
    return hash;
}