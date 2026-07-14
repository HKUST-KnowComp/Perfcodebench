#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    const uint32_t*  off   = offsets.data();
    const std::size_t nseg = offsets.size() - 1;

    uint64_t hash = 14695981039346656037ULL;

    for (int iter = 0; iter < iters; ++iter) {
        // re-initialize hash for this iteration
        uint64_t h = 14695981039346656037ULL;

        for (std::size_t s = 0; s < nseg; ++s) {
            const uint32_t len = off[s + 1] - off[s];
            const uint32_t id  = static_cast<uint32_t>(s);

            // unroll 4× to hide mix latency
            std::size_t i = 0;
            for (; i + 3 < len; i += 4) {
                h = mix(h, id);
                h = mix(h, id);
                h = mix(h, id);
                h = mix(h, id);
            }
            for (; i < len; ++i) {
                h = mix(h, id);
            }
        }
        hash = h;
    }
    return hash;
}