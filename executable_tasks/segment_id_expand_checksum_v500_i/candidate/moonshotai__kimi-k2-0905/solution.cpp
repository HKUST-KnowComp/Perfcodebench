#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    const std::size_t n = offsets.size();
    uint64_t hash = 14695981039346656037ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 14695981039346656037ULL;
        for (std::size_t s = 0; s + 1 < n; ++s) {
            const uint32_t len = offsets[s + 1] - offsets[s];
            for (uint32_t i = 0; i < len; ++i) {
                h = mix(h, static_cast<uint64_t>(s));
            }
        }
        hash = h;
    }
    return hash;
}