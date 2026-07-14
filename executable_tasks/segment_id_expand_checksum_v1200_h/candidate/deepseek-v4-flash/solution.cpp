#include "interface.h"

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (iters <= 0) return 0;
    const uint64_t fnv_offset_basis = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    uint64_t hash = fnv_offset_basis;
    for (size_t s = 0; s + 1 < offsets.size(); ++s) {
        uint64_t val = static_cast<uint64_t>(s);
        uint32_t len = offsets[s+1] - offsets[s];
        for (uint32_t i = 0; i < len; ++i) {
            hash ^= val;
            hash *= fnv_prime;
        }
    }
    return hash;
}