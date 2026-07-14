#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    std::vector<uint64_t> accum(domain, 0);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Reset accumulator
        for (uint32_t i = 0; i < domain; ++i) accum[i] = 0;
        // Accumulate updates
        for (std::size_t i = 0; i < indices.size(); ++i) {
            accum[indices[i]] += static_cast<uint64_t>(values[i]);
        }
        // Compute checksum
        hash = 1469598103934665603ULL;
        for (uint32_t k = 0; k < domain; ++k) {
            uint64_t v = accum[k];
            hash ^= v;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}