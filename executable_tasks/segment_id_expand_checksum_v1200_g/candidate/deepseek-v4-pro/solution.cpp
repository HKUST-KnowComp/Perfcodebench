#include "interface.h"

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
        const uint64_t value = static_cast<uint64_t>(s);
        uint64_t len = static_cast<uint64_t>(offsets[s + 1]) - offsets[s];
        for (uint64_t i = 0; i < len; ++i) {
            hash ^= value;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}