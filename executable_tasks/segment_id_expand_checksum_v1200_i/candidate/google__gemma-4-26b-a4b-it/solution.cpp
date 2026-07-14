#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (offsets.size() < 2) {
        // If there are no segments to expand, the loop logic implies 
        // the hash is reset to the FNV offset basis each iteration.
        // Based on the baseline, if expanded is empty, hash becomes 1469598103934665603ULL.
        return (iters > 0) ? 1469598103934665603ULL : 0;
    }

    uint64_t hash = 1469598103934665603ULL;
    
    // Pre-calculate segment lengths and indices to avoid repeated subtraction
    struct Segment {
        uint32_t index;
        uint32_t length;
    };
    
    std::vector<Segment> segments;
    segments.reserve(offsets.size() - 1);
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
        uint32_t len = offsets[s + 1] - offsets[s];
        if (len > 0) {
            segments.push_back({static_cast<uint32_t>(s), len});
        }
    }

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (const auto& seg : segments) {
            const uint64_t val = seg.index;
            const uint32_t len = seg.length;
            // Unrolling or tight loop for the segment length
            for (uint32_t i = 0; i < len; ++i) {
                hash ^= val;
                hash *= 1099511628211ULL;
            }
        }
    }

    return hash;
}