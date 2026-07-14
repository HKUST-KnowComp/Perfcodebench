#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    uint64_t hash = 0;
    const size_t num_segments = offsets.size() > 0 ? offsets.size() - 1 : 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t s = 0; s < num_segments; ++s) {
            const uint32_t len = offsets[s + 1] - offsets[s];
            const uint64_t seg_id = static_cast<uint64_t>(s);
            for (uint32_t i = 0; i < len; ++i) {
                hash = (hash ^ seg_id) * FNV_PRIME;
            }
        }
    }
    return hash;
}