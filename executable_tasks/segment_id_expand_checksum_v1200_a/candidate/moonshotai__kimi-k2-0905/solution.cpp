#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    const std::size_t n = offsets.size();
    if (n < 2) return 0;

    uint64_t hash = 14695981039346656037ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 14695981039346656037ULL;

        for (std::size_t s = 0; s + 1 < n; ++s) {
            const uint32_t len = offsets[s + 1] - offsets[s];
            const uint64_t val = static_cast<uint64_t>(s);

            // unroll 4× to reduce loop overhead
            std::size_t k = len & ~std::size_t(3);
            for (std::size_t i = 0; i < k; i += 4) {
                h = mix(h, val);
                h = mix(h, val);
                h = mix(h, val);
                h = mix(h, val);
            }
            for (std::size_t i = k; i < len; ++i) {
                h = mix(h, val);
            }
        }

        hash = h;
    }

    return hash;
}