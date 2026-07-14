#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    const auto sz = offsets.size();
    for (std::size_t s = 0; s + 1 < sz; ++s) {
        uint32_t len = offsets[s + 1] - offsets[s];
        uint32_t value = static_cast<uint32_t>(s);
        for (uint32_t i = 0; i < len; ++i) {
            hash ^= value;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}