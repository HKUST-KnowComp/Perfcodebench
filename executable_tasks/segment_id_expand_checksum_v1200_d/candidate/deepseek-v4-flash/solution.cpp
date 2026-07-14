#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (iters <= 0) return 0ULL;
    uint64_t hash = 1469598103934665603ULL; // FNV offset basis
    const uint64_t prime = 1099511628211ULL;
    const size_t n = offsets.size();
    for (size_t s = 0; s + 1 < n; ++s) {
        uint32_t len = offsets[s + 1] - offsets[s];
        uint32_t segment = static_cast<uint32_t>(s);
        for (uint32_t i = 0; i < len; ++i) {
            hash ^= segment;
            hash *= prime;
        }
    }
    return hash;
}
