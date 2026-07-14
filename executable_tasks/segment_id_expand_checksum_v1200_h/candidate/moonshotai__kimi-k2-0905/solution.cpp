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
    const uint32_t* off = offsets.data();
    const std::size_t n = offsets.size();

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (std::size_t s = 0; s + 1 < n; ++s) {
            const uint32_t len = off[s + 1] - off[s];
            const uint32_t id  = static_cast<uint32_t>(s);
            for (uint32_t i = 0; i < len; ++i) {
                h = mix(h, id);
            }
        }
        hash = h;
    }
    return hash;
}