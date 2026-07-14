#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) noexcept {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values,
                            uint32_t bins, int iters) {
    if (bins == 0 || iters == 0) return 0;

    // Use a fixed-size vector instead of unordered_map
    std::vector<uint32_t> cnt(bins, 0u);

    const uint32_t* __restrict data = values.data();
    const size_t n = values.size();

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        // Zero bins in one memset instead of a loop
        std::memset(cnt.data(), 0, bins * sizeof(uint32_t));

        // Accumulate histogram
        for (size_t i = 0; i < n; ++i) {
            const uint32_t v = data[i];
            if (v < bins) ++cnt[v];
        }

        // Re-initialize hash each iteration
        hash = 1469598103934665603ULL;

        // Mix bins into hash.  4-at-a-time unroll for ILP & vectorization.
        uint32_t* __restrict p = cnt.data();
        const uint32_t* end4 = p + (bins & ~3u);
        while (p < end4) {
            hash = mix(hash, p[0]);
            hash = mix(hash, p[1]);
            hash = mix(hash, p[2]);
            hash = mix(hash, p[3]);
            p += 4;
        }
        // Handle remaining 0-3 bins
        for (uint32_t b = (bins & ~3u); b < bins; ++b) {
            hash = mix(hash, cnt[b]);
        }
    }
    return hash;
}