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
    const size_t segs = offsets.size() - 1;
    std::vector<uint32_t> lens;
    lens.reserve(segs);
    uint64_t total = 0;
    for (size_t s = 0; s < segs; ++s) {
        uint32_t l = offsets[s + 1] - offsets[s];
        lens.push_back(l);
        total += l;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 14695981039346656037ULL;
        for (size_t s = 0; s < segs; ++s) {
            const uint32_t id = static_cast<uint32_t>(s);
            uint32_t cnt = lens[s];
            while (cnt--) {
                hash = mix(hash, id);
            }
        }
    }
    return hash;
}